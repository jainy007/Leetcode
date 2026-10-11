#include "ea/plant/acc_plant.hpp"

#include <cmath>

#include "ea/codec/bits.hpp"
#include "ea/core/assert.hpp"
#include "ea/plant/acc_can.hpp"

namespace ea::plant {

namespace {

constexpr std::int64_t kTickMs = 20;
constexpr std::int64_t kRadarPeriodMs = 40;
constexpr std::int64_t kButtonHoldMs = 100;

double clampd(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Physical -> raw with round-half-away and saturation to the signal's raw range.
void put(std::array<std::uint8_t, 8>& f, const can::Sig& s, double phys) {
    const double raw_d = std::round(phys / s.factor);
    const std::int64_t lo = s.is_signed ? -(std::int64_t{1} << (s.length - 1U)) : 0;
    const std::int64_t hi = s.is_signed ? (std::int64_t{1} << (s.length - 1U)) - 1 : (std::int64_t{1} << s.length) - 1;
    std::int64_t raw = static_cast<std::int64_t>(clampd(raw_d, static_cast<double>(lo), static_cast<double>(hi)));
    const std::uint64_t bits = static_cast<std::uint64_t>(raw) & ((std::uint64_t{1} << s.length) - 1U);
    const Result<void> r = codec::insert(ByteSpan(f.data(), f.size()), can::kOrder, s.start, s.length, bits);
    EA_ASSERT(r.ok());
}

std::int64_t get_raw(ConstByteSpan f, const can::Sig& s) {
    const Result<std::uint64_t> r = codec::extract(f, can::kOrder, s.start, s.length);
    EA_ASSERT(r.ok());
    return s.is_signed ? codec::sign_extend(*r, s.length) : static_cast<std::int64_t>(*r);
}

}  // namespace

AccPlant::AccPlant() noexcept
    : radar_tx_(can::kRadarE2e), ego_tx_(can::kEgoE2e), driver_tx_(can::kDriverE2e), cmd_rx_(can::kCommandE2e) {
    for (auto& b : button_until_ms_) {
        b = -1;
    }
}

std::int32_t AccPlant::accel_raw(double mps2) noexcept {
    return static_cast<std::int32_t>(std::llround(mps2 / 0.01));
}

std::int32_t AccPlant::jerk_step_raw(double mps3) noexcept {
    return static_cast<std::int32_t>(std::llround(mps3 * kTickS / 0.01));
}

void AccPlant::set_ego_speed_kph(double v) noexcept {
    ego_v_ = v / 3.6;
    ego_a_ = 0.0;
}

void AccPlant::set_lead(double distance_m, double speed_kph, double lateral_m) noexcept {
    lead_present_ = true;
    lead_x_ = ego_x_ + distance_m;
    lead_v_ = speed_kph / 3.6;
    lead_target_v_ = lead_v_;
    lead_accel_ = 0.0;
    lead_lat_ = lateral_m;
}

void AccPlant::remove_lead() noexcept {
    lead_present_ = false;
}

void AccPlant::lead_change_speed(double target_kph, double accel_mps2) noexcept {
    lead_target_v_ = target_kph / 3.6;
    lead_accel_ = std::fabs(accel_mps2);
}

void AccPlant::cut_in(double distance_m, double speed_kph) noexcept {
    set_lead(distance_m, speed_kph, 0.0);
}

void AccPlant::mark_driver_action() noexcept {
    action_ms_ = now_ms_;
    status_at_action_ = cmd_.status;
    latency_ms_ = -1.0;
}

void AccPlant::set_main_switch(bool on) noexcept {
    main_switch_ = on;
    mark_driver_action();
}

void AccPlant::press(Button b) noexcept {
    button_until_ms_[static_cast<std::size_t>(b)] = now_ms_ + kButtonHoldMs;
    mark_driver_action();
}

void AccPlant::set_brake_pedal(bool pressed) noexcept {
    brake_pedal_ = pressed;
    mark_driver_action();
}

void AccPlant::set_accel_pedal(double pct) noexcept {
    accel_pedal_pct_ = clampd(pct, 0.0, 100.0);
    mark_driver_action();
}

void AccPlant::set_gap_setting(std::uint32_t setting) noexcept {
    gap_setting_ = setting;
}

void AccPlant::lose_frames(Stream s, std::uint32_t n) noexcept {
    lose_[static_cast<std::size_t>(s)] += n;
}

void AccPlant::corrupt_frames(Stream s, std::uint32_t n) noexcept {
    corrupt_[static_cast<std::size_t>(s)] += n;
}

void AccPlant::skip_frames(Stream s, std::uint32_t n) noexcept {
    skip_[static_cast<std::size_t>(s)] += n;
}

void AccPlant::send(ControllerPort& sut, Stream s, std::uint32_t id, std::array<std::uint8_t, 8>& f,
                    e2e::P11Protector& prot) noexcept {
    const auto i = static_cast<std::size_t>(s);
    if (skip_[i] > 0U) {  // sender pause: nothing transmitted, counter not consumed
        --skip_[i];
        return;
    }
    const Result<void> p = prot.protect(ByteSpan(f.data(), f.size()));  // counter advances even if lost on the bus
    EA_ASSERT(p.ok());
    if (lose_[i] > 0U) {
        --lose_[i];
        return;
    }
    if (corrupt_[i] > 0U) {
        --corrupt_[i];
        f[5] = static_cast<std::uint8_t>(f[5] ^ 0x04U);  // payload bit flip after protection
    }
    sut.on_frame(id, ConstByteSpan(f.data(), f.size()));
}

void AccPlant::decode_command(ConstByteSpan f) noexcept {
    ++m_.command_frames;
    if (e2e::is_usable(cmd_rx_.check(f)) == false) {
        ++m_.command_e2e_errors;
        return;
    }
    Command c{};
    const std::int64_t st = get_raw(f, can::command::kAccStatus);
    c.status = (st <= 4) ? static_cast<AccStatus>(st) : AccStatus::kInvalid;
    c.accel_raw = static_cast<std::int32_t>(get_raw(f, can::command::kAccelRequest));
    c.brake_raw = static_cast<std::int32_t>(get_raw(f, can::command::kBrakeRequest));
    c.steering_deg = static_cast<double>(get_raw(f, can::command::kSteeringAngleRequest)) * 0.1;
    c.set_speed_kph = static_cast<double>(get_raw(f, can::command::kSetSpeed));
    c.time_gap_setting = static_cast<std::uint32_t>(get_raw(f, can::command::kTimeGapSetting));

    m_.max_accel_raw = c.accel_raw > m_.max_accel_raw ? c.accel_raw : m_.max_accel_raw;
    m_.max_brake_raw = c.brake_raw > m_.max_brake_raw ? c.brake_raw : m_.max_brake_raw;
    if (c.accel_raw > 0 && c.brake_raw > 0) {
        ++m_.both_requested;
    }
    if (c.steering_deg != 0.0) {
        ++m_.steering_nonzero;
    }
    if (have_prev_cmd_ && prev_cmd_.status == AccStatus::kActive && c.status == AccStatus::kActive) {
        const std::int32_t d = (c.accel_raw - c.brake_raw) - (prev_cmd_.accel_raw - prev_cmd_.brake_raw);
        m_.max_jerk_up_raw = d > m_.max_jerk_up_raw ? d : m_.max_jerk_up_raw;
        m_.max_jerk_down_raw = -d > m_.max_jerk_down_raw ? -d : m_.max_jerk_down_raw;
    }
    if (action_ms_ >= 0 && latency_ms_ < 0.0 && c.status != status_at_action_) {
        latency_ms_ = static_cast<double>(now_ms_ - action_ms_);
    }
    prev_cmd_ = c;
    have_prev_cmd_ = true;
    cmd_ = c;
}

void AccPlant::tick(ControllerPort& sut) noexcept {
    // 1. Frames due at this instant.
    if (now_ms_ % kRadarPeriodMs == 0) {
        std::array<std::uint8_t, 8> f{};
        const double range = lead_x_ - ego_x_;
        const bool visible = lead_present_ && range >= 0.0 && range <= kRadarRangeM;
        put(f, can::radar::kObjValid, visible ? 1.0 : 0.0);
        put(f, can::radar::kObjRange, visible ? range : 0.0);
        put(f, can::radar::kObjRangeRate, visible ? (lead_v_ - ego_v_) : 0.0);
        put(f, can::radar::kObjLateralPos, visible ? lead_lat_ : 0.0);
        send(sut, Stream::kRadar, can::kRadarObjectId, f, radar_tx_);
    }
    {
        std::array<std::uint8_t, 8> f{};
        put(f, can::ego::kVehicleSpeed, ego_v_ * 3.6);
        send(sut, Stream::kEgoState, can::kEgoStateId, f, ego_tx_);
    }
    {
        std::array<std::uint8_t, 8> f{};
        const auto held = [this](Button b) { return now_ms_ < button_until_ms_[static_cast<std::size_t>(b)] ? 1.0 : 0.0; };
        put(f, can::driver::kMainSwitch, main_switch_ ? 1.0 : 0.0);
        put(f, can::driver::kSetButton, held(Button::kSet));
        put(f, can::driver::kResumeButton, held(Button::kResume));
        put(f, can::driver::kCancelButton, held(Button::kCancel));
        put(f, can::driver::kSpeedUpButton, held(Button::kSpeedUp));
        put(f, can::driver::kSpeedDownButton, held(Button::kSpeedDown));
        put(f, can::driver::kGapSetting, static_cast<double>(gap_setting_));
        put(f, can::driver::kBrakePedal, brake_pedal_ ? 1.0 : 0.0);
        put(f, can::driver::kAccelPedalPos, accel_pedal_pct_);
        send(sut, Stream::kDriverInput, can::kDriverInputId, f, driver_tx_);
    }
    // 2. Controller cycle, 3. command frame.
    sut.step();
    decode_command(sut.command_frame());

    // 4. Vehicle dynamics.
    double a_target = 0.0;
    if (brake_pedal_) {
        a_target = -kDriverBrakeMps2;
    } else if (accel_pedal_pct_ > 0.0) {
        a_target = accel_pedal_pct_ * kPedalGainMps2PerPct;
    } else if (cmd_.status == AccStatus::kActive) {
        a_target = static_cast<double>(cmd_.accel_raw - cmd_.brake_raw) * 0.01;
    }
    ego_a_ += (a_target - ego_a_) * kTickS / kActuatorTauS;
    ego_v_ = ego_v_ + ego_a_ * kTickS;
    if (ego_v_ < 0.0) {
        ego_v_ = 0.0;
    }
    ego_x_ += ego_v_ * kTickS;
    if (lead_present_) {
        if (lead_v_ > lead_target_v_) {
            lead_v_ = (lead_v_ - lead_accel_ * kTickS > lead_target_v_) ? lead_v_ - lead_accel_ * kTickS : lead_target_v_;
        } else if (lead_v_ < lead_target_v_) {
            lead_v_ = (lead_v_ + lead_accel_ * kTickS < lead_target_v_) ? lead_v_ + lead_accel_ * kTickS : lead_target_v_;
        }
        lead_x_ += lead_v_ * kTickS;
        if (std::fabs(lead_lat_) <= kLaneHalfWidthM) {
            const double d = lead_x_ - ego_x_;
            m_.min_distance_m = d < m_.min_distance_m ? d : m_.min_distance_m;
            if (d <= 0.0) {
                m_.collision = true;
            }
        }
    }
    const double v_kph = ego_v_ * 3.6;
    m_.max_ego_speed_kph = v_kph > m_.max_ego_speed_kph ? v_kph : m_.max_ego_speed_kph;
    ++m_.ticks;
    now_ms_ += kTickMs;
}

void AccPlant::run(ControllerPort& sut, double seconds) noexcept {
    const auto ticks = static_cast<std::int64_t>(std::llround(seconds * 1000.0)) / kTickMs;
    for (std::int64_t i = 0; i < ticks; ++i) {
        tick(sut);
    }
}

}  // namespace ea::plant

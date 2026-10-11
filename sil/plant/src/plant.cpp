#include "ea/plant/plant.hpp"

#include <cmath>

#include "ea/core/assert.hpp"
#include "ea/plant/signals.hpp"
#include "ea/plant/vehicle_can.hpp"

namespace ea::plant {

namespace {

constexpr std::int64_t kTickMs = 20;
constexpr std::int64_t kRadarPeriodMs = 40;
constexpr std::int64_t kButtonHoldMs = 100;

double clampd(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

}  // namespace

Plant::Plant() noexcept : radar_tx_(can::kRadarE2e), ego_tx_(can::kEgoE2e), driver_tx_(can::kDriverE2e) {
    for (auto& b : button_until_ms_) {
        b = -1;
    }
}

std::int32_t Plant::accel_raw(double mps2) noexcept {
    return static_cast<std::int32_t>(std::llround(mps2 / 0.01));
}

std::int32_t Plant::jerk_step_raw(double mps3) noexcept {
    return static_cast<std::int32_t>(std::llround(mps3 * kTickS / 0.01));
}

void Plant::set_ego_speed_kph(double v) noexcept {
    ego_v_ = v / 3.6;
    ego_a_ = 0.0;
}

void Plant::set_lead(double distance_m, double speed_kph, double lateral_m) noexcept {
    lead_present_ = true;
    lead_x_ = ego_x_ + distance_m;
    lead_v_ = speed_kph / 3.6;
    lead_target_v_ = lead_v_;
    lead_accel_ = 0.0;
    lead_lat_ = lateral_m;
}

void Plant::remove_lead() noexcept {
    lead_present_ = false;
}

void Plant::lead_change_speed(double target_kph, double accel_mps2) noexcept {
    lead_target_v_ = target_kph / 3.6;
    lead_accel_ = std::fabs(accel_mps2);
}

void Plant::cut_in(double distance_m, double speed_kph) noexcept {
    set_lead(distance_m, speed_kph, 0.0);
}

void Plant::mark_driver_action() noexcept {
    action_ms_ = now_ms_;
    status_at_action_ = last_status_;
    latency_ms_ = -1.0;
}

void Plant::set_main_switch(bool on) noexcept {
    main_switch_ = on;
    mark_driver_action();
}

void Plant::press(Button b) noexcept {
    button_until_ms_[static_cast<std::size_t>(b)] = now_ms_ + kButtonHoldMs;
    mark_driver_action();
}

void Plant::set_brake_pedal(bool pressed) noexcept {
    brake_pedal_ = pressed;
    mark_driver_action();
}

void Plant::set_accel_pedal(double pct) noexcept {
    accel_pedal_pct_ = clampd(pct, 0.0, 100.0);
    mark_driver_action();
}

void Plant::set_gap_setting(std::uint32_t setting) noexcept {
    gap_setting_ = setting;
}

void Plant::lose_frames(Stream s, std::uint32_t n) noexcept {
    lose_[static_cast<std::size_t>(s)] += n;
}

void Plant::corrupt_frames(Stream s, std::uint32_t n) noexcept {
    corrupt_[static_cast<std::size_t>(s)] += n;
}

void Plant::skip_frames(Stream s, std::uint32_t n) noexcept {
    skip_[static_cast<std::size_t>(s)] += n;
}

void Plant::send(ControllerPort& sut, Stream s, std::uint32_t id, std::array<std::uint8_t, 8>& f,
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

WorldView Plant::view() const noexcept {
    WorldView w{};
    w.time_s = time_s();
    w.ego_speed_mps = ego_v_;
    w.lead_present = lead_present_;
    w.lead_in_lane = lead_present_ && std::fabs(lead_lat_) <= kLaneHalfWidthM;
    w.distance_m = lead_x_ - ego_x_;
    w.closing_speed_mps = ego_v_ - lead_v_;
    return w;
}

void Plant::tick(ControllerPort& sut, FeatureObserver& obs) noexcept {
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
        const auto held = [this](Button b) {
            return now_ms_ < button_until_ms_[static_cast<std::size_t>(b)] ? 1.0 : 0.0;
        };
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
    // 2. Function cycle, 3. observer.
    sut.step();
    obs.observe(view(), sut);
    const std::int32_t status = obs.status_code();
    if (action_ms_ >= 0 && latency_ms_ < 0.0 && status != status_at_action_) {
        latency_ms_ = static_cast<double>(now_ms_ - action_ms_);
    }
    last_status_ = status;

    // 4. Vehicle dynamics. Driver pedals first; a braking request of the function wins if it
    //    decelerates harder; an acceleration request applies only without pedals.
    double a_target = 0.0;
    if (brake_pedal_) {
        a_target = -kDriverBrakeMps2;
    } else if (accel_pedal_pct_ > 0.0) {
        a_target = accel_pedal_pct_ * kPedalGainMps2PerPct;
    }
    double request = 0.0;
    if (obs.longitudinal_request(request)) {
        if (request < 0.0) {
            a_target = (request < a_target) ? request : a_target;
        } else if (!brake_pedal_ && accel_pedal_pct_ <= 0.0) {
            a_target = request;
        }
    }
    ego_a_ += (a_target - ego_a_) * kTickS / kActuatorTauS;
    ego_v_ = ego_v_ + ego_a_ * kTickS;
    if (ego_v_ < 0.0) {
        ego_v_ = 0.0;
        ego_a_ = (ego_a_ < 0.0) ? 0.0 : ego_a_;
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
            if (d <= 0.0 && !m_.collision) {
                m_.collision = true;
                m_.impact_speed_kph = (ego_v_ - lead_v_) * 3.6;
            }
        }
    }
    const double v_kph = ego_v_ * 3.6;
    m_.max_ego_speed_kph = v_kph > m_.max_ego_speed_kph ? v_kph : m_.max_ego_speed_kph;
    ++m_.ticks;
    now_ms_ += kTickMs;
}

void Plant::run(ControllerPort& sut, FeatureObserver& obs, double seconds) noexcept {
    const auto ticks = static_cast<std::int64_t>(std::llround(seconds * 1000.0)) / kTickMs;
    for (std::int64_t i = 0; i < ticks; ++i) {
        tick(sut, obs);
    }
}

}  // namespace ea::plant

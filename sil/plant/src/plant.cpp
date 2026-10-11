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
    prev_ego_v_ = ego_v_;
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

void Plant::mark_fault() noexcept {
    if (m_.first_fault_s < 0.0) {
        m_.first_fault_s = time_s();
    }
}

void Plant::inject_node_fault(std::uint32_t can_id, NodeFault kind, std::uint32_t n, const e2e::P11Config& e2e,
                              std::uint32_t sig_start, std::uint32_t sig_length, bool sig_signed, double sig_factor,
                              double value) noexcept {
    for (NodeFaultSpec& f : node_faults_) {
        if (f.remaining == 0U || f.can_id == can_id) {
            const bool keep_last = f.can_id == can_id && f.have_last;
            const BusFrame last = f.last;
            f = NodeFaultSpec{};
            f.can_id = can_id;
            f.kind = kind;
            f.remaining = n;
            f.e2e = e2e;
            f.sig_start = sig_start;
            f.sig_length = sig_length;
            f.sig_signed = sig_signed;
            f.sig_factor = sig_factor;
            f.value = value;
            f.have_last = keep_last;
            f.last = last;
            return;
        }
    }
    EA_ASSERT(false);  // more concurrent node faults than supported
}

bool Plant::apply_node_fault(BusFrame& f) noexcept {
    for (NodeFaultSpec& nf : node_faults_) {
        if (nf.can_id != f.can_id) {
            continue;
        }
        if (nf.remaining == 0U) {
            nf.last = f;  // remember the last good frame for a later freeze
            nf.have_last = true;
            return true;
        }
        --nf.remaining;
        mark_fault();
        switch (nf.kind) {
            case NodeFault::kLose:
                return false;
            case NodeFault::kFreeze:
                if (nf.have_last) {
                    f = nf.last;
                }
                return true;
            case NodeFault::kCorrupt:
                f.data[5] = static_cast<std::uint8_t>(f.data[5] ^ 0x04U);
                return true;
            case NodeFault::kOverride: {
                const can::Sig sig{nf.sig_start, nf.sig_length, nf.sig_signed, nf.sig_factor};
                put(f.data, sig, nf.value);
                f.data[nf.e2e.crc_byte] = e2e::p11_crc(nf.e2e, f.payload());
                return true;
            }
        }
    }
    return true;
}

void Plant::send(Span<ControllerPort* const> nodes, Stream s, std::uint32_t id, std::array<std::uint8_t, 8>& f,
                 e2e::P11Protector& prot) noexcept {
    const auto i = static_cast<std::size_t>(s);
    if (skip_[i] > 0U) {  // sender pause: nothing transmitted, counter not consumed
        --skip_[i];
        mark_fault();
        return;
    }
    const Result<void> p = prot.protect(ByteSpan(f.data(), f.size()));  // counter advances even if lost on the bus
    EA_ASSERT(p.ok());
    if (lose_[i] > 0U) {
        --lose_[i];
        mark_fault();
        return;
    }
    if (corrupt_[i] > 0U) {
        --corrupt_[i];
        mark_fault();
        f[5] = static_cast<std::uint8_t>(f[5] ^ 0x04U);  // payload bit flip after protection
    }
    for (ControllerPort* n : nodes) {
        n->on_frame(id, ConstByteSpan(f.data(), f.size()));
    }
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

void Plant::tick(Span<ControllerPort* const> nodes, Span<FeatureObserver* const> observers, std::size_t actuation) noexcept {
    // 1. Frames due at this instant.
    if (now_ms_ % kRadarPeriodMs == 0) {
        std::array<std::uint8_t, 8> f{};
        const double range = lead_x_ - ego_x_;
        const bool visible = lead_present_ && range >= 0.0 && range <= kRadarRangeM;
        put(f, can::radar::kObjValid, visible ? 1.0 : 0.0);
        put(f, can::radar::kObjRange, visible ? range : 0.0);
        put(f, can::radar::kObjRangeRate, visible ? (lead_v_ - ego_v_) : 0.0);
        put(f, can::radar::kObjLateralPos, visible ? lead_lat_ : 0.0);
        send(nodes, Stream::kRadar, can::kRadarObjectId, f, radar_tx_);
    }
    {
        std::array<std::uint8_t, 8> f{};
        put(f, can::ego::kVehicleSpeed, ego_v_ * 3.6);
        send(nodes, Stream::kEgoState, can::kEgoStateId, f, ego_tx_);
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
        send(nodes, Stream::kDriverInput, can::kDriverInputId, f, driver_tx_);
    }
    // 2. Nodes step in order; their frames pass the fault injector and reach the other nodes.
    tick_frame_count_ = 0U;
    for (std::size_t k = 0U; k < nodes.size(); ++k) {
        ControllerPort* node = nodes[k];
        node->step();
        for (std::size_t t = 0U; t < node->tx_count(); ++t) {
            BusFrame bf{};
            bf.can_id = node->tx_can_id(t);
            const ConstByteSpan payload = node->tx_frame(t);
            EA_ASSERT(payload.size() <= bf.data.size());
            for (std::size_t b = 0U; b < payload.size(); ++b) {
                bf.data[b] = payload[b];
            }
            bf.length = payload.size();
            if (!apply_node_fault(bf)) {
                continue;
            }
            EA_ASSERT(tick_frame_count_ < tick_frames_.size());
            tick_frames_[tick_frame_count_++] = bf;
            for (std::size_t j = 0U; j < nodes.size(); ++j) {
                if (j != k) {
                    nodes[j]->on_frame(bf.can_id, bf.payload());
                }
            }
        }
    }
    // 3. Observers.
    const WorldView w = view();
    for (FeatureObserver* o : observers) {
        o->observe(w, Span<const BusFrame>(tick_frames_.data(), tick_frame_count_));
    }
    const std::int32_t status = observers[0]->status_code();
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
    if (observers[actuation]->longitudinal_request(request)) {
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
    const double a_actual = (ego_v_ - prev_ego_v_) / kTickS;
    prev_ego_v_ = ego_v_;
    m_.max_accel_mps2 = a_actual > m_.max_accel_mps2 ? a_actual : m_.max_accel_mps2;
    m_.max_decel_mps2 = -a_actual > m_.max_decel_mps2 ? -a_actual : m_.max_decel_mps2;
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
    ControllerPort* const nodes[] = {&sut};
    FeatureObserver* const observers[] = {&obs};
    run(Span<ControllerPort* const>(nodes), Span<FeatureObserver* const>(observers), 0U, seconds);
}

void Plant::run(Span<ControllerPort* const> nodes, Span<FeatureObserver* const> observers, std::size_t actuation,
                double seconds) noexcept {
    EA_ASSERT(!nodes.empty() && !observers.empty() && actuation < observers.size());
    const auto ticks = static_cast<std::int64_t>(std::llround(seconds * 1000.0)) / kTickMs;
    for (std::int64_t i = 0; i < ticks; ++i) {
        tick(nodes, observers, actuation);
    }
}

}  // namespace ea::plant

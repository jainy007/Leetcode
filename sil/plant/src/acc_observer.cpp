#include "ea/plant/acc_observer.hpp"

#include "ea/plant/acc_can.hpp"
#include "ea/plant/signals.hpp"

namespace ea::plant {

AccObserver::AccObserver() noexcept : rx_(can::kAccCommandE2e) {}

void AccObserver::observe(const WorldView& /*world*/, const ControllerPort& sut) noexcept {
    for (std::size_t i = 0U; i < sut.tx_count(); ++i) {
        if (sut.tx_can_id(i) != can::kAccCommandId) {
            continue;
        }
        const ConstByteSpan f = sut.tx_frame(i);
        ++m_.command_frames;
        if (!e2e::is_usable(rx_.check(f))) {
            ++m_.command_e2e_errors;
            continue;
        }
        AccCommand c{};
        const std::int64_t st = get_raw(f, can::acc_command::kAccStatus);
        c.status = (st <= 4) ? static_cast<AccStatus>(st) : AccStatus::kInvalid;
        c.accel_raw = static_cast<std::int32_t>(get_raw(f, can::acc_command::kAccelRequest));
        c.brake_raw = static_cast<std::int32_t>(get_raw(f, can::acc_command::kBrakeRequest));
        c.steering_deg = get(f, can::acc_command::kSteeringAngleRequest);
        c.set_speed_kph = get(f, can::acc_command::kSetSpeed);
        c.time_gap_setting = static_cast<std::uint32_t>(get_raw(f, can::acc_command::kTimeGapSetting));

        m_.max_accel_raw = c.accel_raw > m_.max_accel_raw ? c.accel_raw : m_.max_accel_raw;
        m_.max_brake_raw = c.brake_raw > m_.max_brake_raw ? c.brake_raw : m_.max_brake_raw;
        if (c.accel_raw > 0 && c.brake_raw > 0) {
            ++m_.both_requested;
        }
        if (c.steering_deg != 0.0) {
            ++m_.steering_nonzero;
        }
        if (have_prev_ && prev_.status == AccStatus::kActive && c.status == AccStatus::kActive) {
            const std::int32_t d = (c.accel_raw - c.brake_raw) - (prev_.accel_raw - prev_.brake_raw);
            m_.max_jerk_up_raw = d > m_.max_jerk_up_raw ? d : m_.max_jerk_up_raw;
            m_.max_jerk_down_raw = -d > m_.max_jerk_down_raw ? -d : m_.max_jerk_down_raw;
        }
        prev_ = c;
        have_prev_ = true;
        cmd_ = c;
    }
}

bool AccObserver::longitudinal_request(double& accel_mps2) const noexcept {
    if (cmd_.status != AccStatus::kActive) {
        return false;
    }
    accel_mps2 = static_cast<double>(cmd_.accel_raw - cmd_.brake_raw) * 0.01;
    return true;
}

}  // namespace ea::plant

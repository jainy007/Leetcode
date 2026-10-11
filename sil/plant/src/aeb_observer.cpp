#include "ea/plant/aeb_observer.hpp"

#include "ea/plant/aeb_can.hpp"
#include "ea/plant/signals.hpp"

namespace ea::plant {

namespace {

double ttc_of(const WorldView& w) {
    return (w.lead_in_lane && w.closing_speed_mps > 0.0) ? w.distance_m / w.closing_speed_mps : 1.0e9;
}

}  // namespace

AebObserver::AebObserver() noexcept : rx_(can::kAebCommandE2e) {}

void AebObserver::observe(const WorldView& world, Span<const BusFrame> frames) noexcept {
    for (const BusFrame& bf : frames) {
        if (bf.can_id != can::kAebCommandId) {
            continue;
        }
        const ConstByteSpan f = bf.payload();
        ++m_.command_frames;
        if (!e2e::is_usable(rx_.check(f))) {
            ++m_.command_e2e_errors;
            continue;
        }
        AebCommand c{};
        const std::int64_t st = get_raw(f, can::aeb_command::kAebStatus);
        c.status = (st <= 4) ? static_cast<AebStatus>(st) : AebStatus::kInvalid;
        c.warning = get_raw(f, can::aeb_command::kFcwWarning) != 0;
        c.brake_raw = static_cast<std::int32_t>(get_raw(f, can::aeb_command::kAebBrakeRequest));
        if (c.warning && !m_.warned) {
            m_.warned = true;
            m_.first_warning_s = world.time_s;
            m_.ttc_at_warning_s = ttc_of(world);
        }
        if (c.brake_raw > 0 && !m_.braked) {
            m_.braked = true;
            m_.first_brake_s = world.time_s;
            m_.ttc_at_brake_s = ttc_of(world);
            m_.first_brake_raw = c.brake_raw;
        }
        if (c.status == AebStatus::kFullBrake && !m_.full_braked) {
            m_.full_braked = true;
            m_.ttc_at_full_brake_s = ttc_of(world);
        }
        if (prev_warning_ && !c.warning && !m_.braked && m_.ttc_at_warning_end_s < 0.0) {
            m_.ttc_at_warning_end_s = ttc_of(world);
        }
        prev_warning_ = c.warning;
        m_.max_brake_raw = c.brake_raw > m_.max_brake_raw ? c.brake_raw : m_.max_brake_raw;
        cmd_ = c;
    }
}

bool AebObserver::longitudinal_request(double& accel_mps2) const noexcept {
    if (cmd_.brake_raw <= 0) {
        return false;
    }
    accel_mps2 = -static_cast<double>(cmd_.brake_raw) * 0.01;
    return true;
}

}  // namespace ea::plant

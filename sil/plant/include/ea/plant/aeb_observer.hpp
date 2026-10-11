// Observer for feature 'aeb': decodes AebCommand and records warning/braking timing against the
// plant's own world state (TTC = distance / closing speed in the simulation, not the radar).
#pragma once

#include <cstdint>

#include "ea/e2e/p11.hpp"
#include "ea/plant/plant.hpp"

namespace ea::plant {

// State codes as specified by REQ-AEB-014 / the DBC comment (not taken from generated code).
enum class AebStatus : std::uint8_t { kStandby = 0, kWarning = 1, kPartialBrake = 2, kFullBrake = 3, kFault = 4, kInvalid = 255 };

struct AebCommand {
    AebStatus status = AebStatus::kInvalid;
    bool warning = false;
    std::int32_t brake_raw = 0;  // 0.01 m/s2
};

struct AebMetrics {
    bool warned = false;
    double first_warning_s = -1.0;
    double ttc_at_warning_s = -1.0;
    bool braked = false;
    double first_brake_s = -1.0;
    double ttc_at_brake_s = -1.0;
    std::int32_t first_brake_raw = 0;      // deceleration of the first braking stage
    bool full_braked = false;
    double ttc_at_full_brake_s = -1.0;     // first frame with state FULL_BRAKE
    double ttc_at_warning_end_s = -1.0;    // first warning end before any braking (1e9: target not approaching)
    std::int32_t max_brake_raw = 0;
    std::uint32_t command_frames = 0;
    std::uint32_t command_e2e_errors = 0;
};

class AebObserver final : public FeatureObserver {
public:
    AebObserver() noexcept;
    void observe(const WorldView& world, const ControllerPort& sut) noexcept override;
    bool longitudinal_request(double& accel_mps2) const noexcept override;
    std::int32_t status_code() const noexcept override { return static_cast<std::int32_t>(cmd_.status); }

    const AebCommand& last_command() const noexcept { return cmd_; }
    const AebMetrics& metrics() const noexcept { return m_; }

private:
    e2e::P11Checker rx_;
    AebCommand cmd_{};
    bool prev_warning_ = false;
    AebMetrics m_{};
};

}  // namespace ea::plant

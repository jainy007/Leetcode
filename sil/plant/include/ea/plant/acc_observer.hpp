// Observer for feature 'acc': decodes AccCommand with the plant's own layout and E2E check.
#pragma once

#include <cstdint>

#include "ea/e2e/p11.hpp"
#include "ea/plant/plant.hpp"

namespace ea::plant {

// State codes as specified by REQ-ACC-020 / the DBC comment (not taken from generated code).
enum class AccStatus : std::uint8_t { kOff = 0, kStandby = 1, kActive = 2, kOverride = 3, kFault = 4, kInvalid = 255 };

struct AccCommand {
    AccStatus status = AccStatus::kInvalid;
    std::int32_t accel_raw = 0;  // 0.01 m/s2
    std::int32_t brake_raw = 0;  // 0.01 m/s2
    double steering_deg = 0.0;
    double set_speed_kph = 0.0;
    std::uint32_t time_gap_setting = 0U;
};

struct AccMetrics {
    std::int32_t max_accel_raw = 0;
    std::int32_t max_brake_raw = 0;
    std::int32_t max_jerk_up_raw = 0;    // largest increase of (accel - brake) per tick while ACTIVE
    std::int32_t max_jerk_down_raw = 0;  // largest decrease per tick while ACTIVE
    std::uint32_t both_requested = 0;
    std::uint32_t steering_nonzero = 0;
    std::uint32_t command_frames = 0;
    std::uint32_t command_e2e_errors = 0;
};

class AccObserver final : public FeatureObserver {
public:
    AccObserver() noexcept;
    void observe(const WorldView& world, Span<const BusFrame> frames) noexcept override;
    bool longitudinal_request(double& accel_mps2) const noexcept override;
    std::int32_t status_code() const noexcept override { return static_cast<std::int32_t>(cmd_.status); }

    const AccCommand& last_command() const noexcept { return cmd_; }
    const AccMetrics& metrics() const noexcept { return m_; }

private:
    e2e::P11Checker rx_;
    AccCommand cmd_{};
    AccCommand prev_{};
    bool have_prev_ = false;
    AccMetrics m_{};
};

}  // namespace ea::plant

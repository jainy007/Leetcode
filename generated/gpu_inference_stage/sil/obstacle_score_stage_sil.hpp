// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   gpu_inference_stage (pattern: gpu_stage)
// Inputs:    sha256:dba8099d3d048e00e6065ef9d014a96b61ec230c8a485f40da046c2b8e79e124
// Regenerate: python3 -m tools.gen

#pragma once

#include <unistd.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <optional>

#include "ea/core/assert.hpp"
#include "ea/gpu/cpu_ref_backend.hpp"
#include "ea/ipc/shm_channel.hpp"
#include "ea/sil/fault_injecting_backend.hpp"
#include "ea/sil/temp_dir.hpp"
#include "ea/sil/virtual_clock.hpp"
#include "gpu_inference_stage/obstacle_score_monitor.hpp"
#include "gpu_inference_stage/obstacle_score_stage.hpp"

#ifndef EA_SIL_MODEL_DIR
#error "EA_SIL_MODEL_DIR must name the model directory (set by the generated CMakeLists.txt)"
#endif

namespace adas::obstacle_score {

// SIL harness: cpu_ref backend behind a fault injector, virtual clock, weight files copied into a
// private temp dir (so tests may corrupt them), and a real shared-memory channel to the monitor.
class ObstacleScoreStageSil {
public:
    ObstacleScoreStageSil() noexcept
        : cpu_(ea::ByteSpan(arena_.data(), arena_.size())),
          backend_(cpu_, clock_, ObstacleScoreStage::kOutputTensor),
          stage_(backend_, clock_),
          monitor_(clock_) {
        clock_.set(ea::osal::kNsPerSec);
        EA_ASSERT(temp_.copy_from(EA_SIL_MODEL_DIR, ObstacleScoreStage::weight_file(Weight::kFc1W)));
        EA_ASSERT(temp_.copy_from(EA_SIL_MODEL_DIR, ObstacleScoreStage::weight_file(Weight::kFc1B)));
        static std::atomic<unsigned> instance{0U};
        (void)std::snprintf(channel_, sizeof(channel_), "/ea_obstacle_score_sil_%d_%u", static_cast<int>(getpid()),
                            instance.fetch_add(1U));
        auto pub = ea::ipc::ShmPublisher<Output>::create(channel_);
        EA_ASSERT(pub.ok());
        publisher_.emplace(std::move(pub).value());
        auto sub = ea::ipc::ShmSubscriber<Output>::open(channel_);
        EA_ASSERT(sub.ok());
        subscriber_.emplace(std::move(sub).value());
    }

    ea::Result<void> init() noexcept { return stage_.init(temp_.path()); }
    void corrupt_weight_file(Weight w) noexcept { EA_ASSERT(temp_.flip_byte(ObstacleScoreStage::weight_file(w), 0U)); }

    void process_ramp(int n) noexcept {
        Input in{};
        for (std::size_t i = 0U; i < in.size(); ++i) {
            in[i] = static_cast<std::uint8_t>((i * 4U) & 0xFFU);
        }
        for (int k = 0; k < n; ++k) {
            process(in);
        }
    }
    void process_constant(unsigned value) noexcept {
        Input in{};
        in.fill(static_cast<std::uint8_t>(value));
        process(in);
    }

    const Output& last_output() const noexcept { return last_; }
    std::uint32_t allocations_since_init() const noexcept {
        return cpu_.allocation_count() - stage_.allocations_at_init();
    }
    ea::sil::FaultInjectingBackend& backend() noexcept { return backend_; }

    void corrupt_channel(std::size_t cls, float value) noexcept {
        channel_fault_index_ = cls;
        channel_fault_value_ = value;
        channel_fault_ = true;
    }

    void monitor_check(int ms) noexcept {
        clock_.advance(static_cast<ea::osal::Nanoseconds>(ms) * ea::osal::kNsPerMs);
        const ea::Result<Output> latest = subscriber_->latest();
        verdict_ = monitor_.check(latest.ok() ? &latest.value() : nullptr);
    }
    MonitorVerdict last_verdict() const noexcept { return verdict_; }

private:
    void process(const Input& in) noexcept {
        last_ = stage_.run(in);
        Output published = last_;
        if (channel_fault_ && channel_fault_index_ < published.scores.size()) {
            published.scores[channel_fault_index_] = channel_fault_value_;
        }
        publisher_->publish(published);
    }

    alignas(64) std::array<std::uint8_t, ObstacleScoreStage::kArenaBytes> arena_{};
    ea::sil::VirtualClock clock_;
    ea::gpu::CpuRefBackend cpu_;
    ea::sil::FaultInjectingBackend backend_;
    ObstacleScoreStage stage_;
    ObstacleScoreMonitor monitor_;
    ea::sil::TempDir temp_;
    char channel_[64] = {};
    std::optional<ea::ipc::ShmPublisher<Output>> publisher_;
    std::optional<ea::ipc::ShmSubscriber<Output>> subscriber_;
    Output last_{};
    MonitorVerdict verdict_ = MonitorVerdict::kNoData;
    bool channel_fault_ = false;
    std::size_t channel_fault_index_ = 0U;
    float channel_fault_value_ = 0.0F;
};

}  // namespace adas::obstacle_score

// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:806ba8401a33e8aaf8e2f047beee1d3d76f3fb698140f2dca39ffd748ea3c4b4
// Regenerate: python3 -m tools.gen

#pragma once

#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/core/span.hpp"
#include "ea/osal/clock.hpp"
#include "ea/osal/periodic_task.hpp"
#include "ea/supervision/supervision.hpp"
#include "adas_input_gate/adas_input_gate.hpp"

namespace adas::input_gate {

// Provides the frame received since the previous cycle (empty span if none).
class IFrameSource {
public:
    virtual ~IFrameSource() = default;
    virtual ea::ConstByteSpan poll() noexcept = 0;

protected:
    IFrameSource() = default;
    IFrameSource(const IFrameSource&) = default;
    IFrameSource& operator=(const IFrameSource&) = default;
};

// Sends the status message payload (GateStatus).
class IStatusSink {
public:
    virtual ~IStatusSink() = default;
    virtual void send(ea::ConstByteSpan payload) noexcept = 0;

protected:
    IStatusSink() = default;
    IStatusSink(const IStatusSink&) = default;
    IStatusSink& operator=(const IStatusSink&) = default;
};

// Binds AdasInputGate to a periodic real-time task (CPU-006, CPU-007) with deadline supervision.
class AdasInputGateTask {
public:
    static constexpr ea::osal::TaskConfig kConfig{"gate_task", 20 * ea::osal::kNsPerMs, 60, 0x4U};
    static constexpr ea::osal::Nanoseconds kWcetBudget = 500 * ea::osal::kNsPerUs;

    AdasInputGateTask(AdasInputGate& component, IFrameSource& source, IStatusSink& sink, const ea::osal::IClock& clock) noexcept;

    ea::Result<void> start() noexcept;
    void stop() noexcept;

    // One activation; called by the task thread, or directly by SIL with a virtual clock.
    void cycle() noexcept;

    const ea::sup::DeadlineSupervision& deadline() const noexcept { return deadline_; }
    const ea::osal::PeriodicTask& task() const noexcept { return task_; }

private:
    static void entry(void* self) noexcept;

    AdasInputGate& component_;
    IFrameSource& source_;
    IStatusSink& sink_;
    const ea::osal::IClock& clock_;
    ea::sup::DeadlineSupervision deadline_{0, kWcetBudget};
    ea::osal::PeriodicTask task_;
};

}  // namespace adas::input_gate

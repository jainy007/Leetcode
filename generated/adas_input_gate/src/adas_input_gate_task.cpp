// GENERATED FILE - DO NOT EDIT. Changes are overwritten and rejected by CI.
// Generator: ea-gen 0.1.0 (rule-based, deterministic; no AI in this path)
// Project:   adas_input_gate (pattern: input_gate)
// Inputs:    sha256:6b014bf1c6088b1ee8b025ad45997456f3661570e6db76f02157c9ff08730979
// Regenerate: python3 -m tools.gen

#include "adas_input_gate/adas_input_gate_task.hpp"

namespace adas::input_gate {

AdasInputGateTask::AdasInputGateTask(AdasInputGate& component, IFrameSource& source,
        IStatusSink& sink,
        const ea::osal::IClock& clock) noexcept
    : component_(component), source_(source), sink_(sink), clock_(clock) {}

ea::Result<void> AdasInputGateTask::start() noexcept {
    return task_.start(kConfig, &AdasInputGateTask::entry, this);
}

void AdasInputGateTask::stop() noexcept {
    task_.stop();
}

void AdasInputGateTask::entry(void* self) noexcept {
    static_cast<AdasInputGateTask*>(self)->cycle();
}

void AdasInputGateTask::cycle() noexcept {
    deadline_.start(clock_.now());
    component_.step(source_.poll());
    sink_.send(component_.status_payload());
    (void)deadline_.stop(clock_.now());  // violations are counted and read by the health monitor
}

}  // namespace adas::input_gate

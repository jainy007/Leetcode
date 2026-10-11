#include <benchmark/benchmark.h>

#include <array>
#include <optional>

#include "adas_input_gate/adas_input_gate.hpp"
#include "ea/gpu/cpu_ref_backend.hpp"
#include "ea/osal/clock.hpp"
#include "gpu_inference_stage/obstacle_score_stage.hpp"
#include "sil/adas_input_gate_sil.hpp"

namespace {

using namespace adas::input_gate;  // NOLINT

// Builds `count` consecutive valid frames with the generated SIL encoder + E2E protector.
template <std::size_t Count>
std::array<AdasInputGateSil::Frame, Count> make_frames() {
    std::array<AdasInputGateSil::Frame, Count> frames{};
    ea::e2e::P4Protector tx(kObjectSummaryE2e);
    ObjectSummary v{3.0, 42.5, -3.25, 87.5};
    for (std::size_t i = 0; i < Count; ++i) {
        auto& f = frames[i];
        const ea::ByteSpan payload(f.data() + ea::someip::kHeaderSize, kObjectSummaryPayloadLength);
        (void)encode_object_summary(v, payload);
        (void)tx.protect(payload);
        (void)ea::someip::write_header(f, kObjectSummarySomeIp, static_cast<std::uint16_t>(i), payload.size());
    }
    return frames;
}

void BM_DecodeObjectSummary(benchmark::State& st) {
    const auto frames = make_frames<1>();
    const ea::ConstByteSpan payload(frames[0].data() + ea::someip::kHeaderSize, kObjectSummaryPayloadLength);
    for (auto _ : st) {
        auto r = decode_object_summary(payload);
        benchmark::DoNotOptimize(r);
    }
}
BENCHMARK(BM_DecodeObjectSummary);

void BM_EncodeGateStatus(benchmark::State& st) {
    std::array<std::uint8_t, kGateStatusPayloadLength> out{};
    GateStatus s{1.0, 1.0, 5.0, 42.5, 3.0};
    for (auto _ : st) {
        benchmark::DoNotOptimize(encode_gate_status(s, out));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_EncodeGateStatus);

// One full 20 ms cycle of the gate with a valid frame: SOME/IP + E2E + decode + debounce + SM + status.
void BM_GateStepValid(benchmark::State& st) {
    constexpr std::size_t kN = 4096;
    static const auto frames = make_frames<kN>();
    std::optional<AdasInputGate> gate;
    gate.emplace();
    std::size_t i = 0;
    for (auto _ : st) {
        gate->step(ea::ConstByteSpan(frames[i].data(), frames[i].size()));
        auto* g = &*gate;
        benchmark::DoNotOptimize(g);
        if (++i == kN) {
            st.PauseTiming();
            gate.emplace();  // counter sequence restarts at 0
            i = 0;
            st.ResumeTiming();
        }
    }
}
BENCHMARK(BM_GateStepValid);

void BM_GateStepMissing(benchmark::State& st) {
    AdasInputGate gate;
    for (auto _ : st) {
        gate.step(ea::ConstByteSpan());
        auto* g = &gate;
        benchmark::DoNotOptimize(g);
    }
}
BENCHMARK(BM_GateStepMissing);

void BM_ObstacleStageRun(benchmark::State& st) {
    using adas::obstacle_score::Input;
    using adas::obstacle_score::ObstacleScoreStage;
    alignas(64) static std::array<std::uint8_t, ObstacleScoreStage::kArenaBytes> arena{};
    const ea::osal::MonotonicClock clock;
    ea::gpu::CpuRefBackend be(arena);
    static ObstacleScoreStage stage(be, clock);
    if (!stage.initialized() && !stage.init(EA_BENCH_MODEL_DIR).ok()) {
        st.SkipWithError("init failed");
        return;
    }
    Input in{};
    for (std::size_t i = 0; i < in.size(); ++i) {
        in[i] = static_cast<std::uint8_t>(i * 4U);
    }
    for (auto _ : st) {
        auto out = stage.run(in);
        benchmark::DoNotOptimize(out);
    }
}
BENCHMARK(BM_ObstacleStageRun);

}  // namespace

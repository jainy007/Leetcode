// Producer process (QM): runs ObstacleScoreStage periodically and publishes every output to the
// shared-memory channel. Deployment glue, hand-written; the stage itself is generated.
//
//   obstacle_score_stage_main --channel /name --model-dir DIR --frames N --period-ms P
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "ea/gpu/cpu_ref_backend.hpp"
#include "ea/ipc/shm_channel.hpp"
#include "ea/osal/clock.hpp"
#include "ea/osal/periodic_task.hpp"
#include "gpu_inference_stage/obstacle_score_stage.hpp"

namespace {

using adas::obstacle_score::Input;
using adas::obstacle_score::ObstacleScoreStage;
using adas::obstacle_score::Output;

struct Context {
    ObstacleScoreStage* stage;
    ea::ipc::ShmPublisher<Output>* publisher;
    Input input;
    std::atomic<int> remaining;
};

void cycle(void* p) noexcept {
    auto* ctx = static_cast<Context*>(p);
    if (ctx->remaining.load() <= 0) {
        return;
    }
    ctx->publisher->publish(ctx->stage->run(ctx->input));
    ctx->remaining.fetch_sub(1);
}

const char* arg(int argc, char** argv, const char* key, const char* fallback) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::strcmp(argv[i], key) == 0) {
            return argv[i + 1];
        }
    }
    return fallback;
}

alignas(64) std::uint8_t g_arena[ObstacleScoreStage::kArenaBytes];

}  // namespace

int main(int argc, char** argv) {
    const char* channel = arg(argc, argv, "--channel", "/ea_obstacle_score");
    const char* model_dir = arg(argc, argv, "--model-dir", ".");
    const int frames = std::atoi(arg(argc, argv, "--frames", "50"));
    const int period_ms = std::atoi(arg(argc, argv, "--period-ms", "20"));

    const ea::osal::MonotonicClock clock;
    ea::gpu::CpuRefBackend backend(ea::ByteSpan(g_arena, sizeof(g_arena)));
    ObstacleScoreStage stage(backend, clock);
    const ea::Result<void> init = stage.init(model_dir);
    if (!init.ok()) {
        std::fprintf(stderr, "stage: init failed: %s\n", ea::to_string(init.error()));
        return 2;
    }
    auto pub = ea::ipc::ShmPublisher<Output>::create(channel);
    if (!pub.ok()) {
        std::fprintf(stderr, "stage: cannot create channel %s\n", channel);
        return 3;
    }
    Context ctx{&stage, &pub.value(), Input{}, {frames}};
    for (std::size_t i = 0U; i < ctx.input.size(); ++i) {
        ctx.input[i] = static_cast<std::uint8_t>((i * 4U) & 0xFFU);
    }
    ea::osal::PeriodicTask task;
    const ea::osal::TaskConfig cfg{"obs_stage", period_ms * ea::osal::kNsPerMs, 0, 0U};
    if (!task.start(cfg, &cycle, &ctx).ok()) {
        return 4;
    }
    while (ctx.remaining.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
    }
    task.stop();
    std::printf("stage: published %d outputs, overruns %llu\n", frames,
                static_cast<unsigned long long>(task.overruns()));
    return 0;
}

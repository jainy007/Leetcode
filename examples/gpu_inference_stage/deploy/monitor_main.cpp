// Monitor process (ASIL B): opens the channel read-only in logic, checks the latest output every
// period with ObstacleScoreMonitor, and reports verdict counts. Links only the monitor library and
// the shared types, never the producer (MP-001).
//
//   obstacle_score_monitor_main --channel /name --duration-ms D --period-ms P
// Exit code 0 when it saw OK verdicts while the producer ran and TIMEOUT after it stopped.
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <thread>

#include "ea/ipc/shm_channel.hpp"
#include "ea/osal/clock.hpp"
#include "gpu_inference_stage/obstacle_score_monitor.hpp"

namespace {

using adas::obstacle_score::MonitorVerdict;
using adas::obstacle_score::ObstacleScoreMonitor;
using adas::obstacle_score::Output;

const char* arg(int argc, char** argv, const char* key, const char* fallback) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::strcmp(argv[i], key) == 0) {
            return argv[i + 1];
        }
    }
    return fallback;
}

}  // namespace

int main(int argc, char** argv) {
    const char* channel = arg(argc, argv, "--channel", ObstacleScoreMonitor::kChannelName);
    const int duration_ms = std::atoi(arg(argc, argv, "--duration-ms", "2000"));
    const int period_ms = std::atoi(arg(argc, argv, "--period-ms", "10"));

    std::optional<ea::ipc::ShmSubscriber<Output>> sub;
    for (int attempt = 0; attempt < 200 && !sub; ++attempt) {  // bounded wait for the producer
        auto s = ea::ipc::ShmSubscriber<Output>::open(channel);
        if (s.ok()) {
            sub.emplace(std::move(s).value());
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    if (!sub) {
        std::fprintf(stderr, "monitor: channel %s never appeared\n", channel);
        return 2;
    }

    const ea::osal::MonotonicClock clock;
    ObstacleScoreMonitor monitor(clock);
    unsigned counts[5] = {0U, 0U, 0U, 0U, 0U};
    MonitorVerdict last = MonitorVerdict::kNoData;
    for (int t = 0; t < duration_ms; t += period_ms) {
        const ea::Result<Output> latest = sub->latest();
        last = monitor.check(latest.ok() ? &latest.value() : nullptr);
        ++counts[static_cast<unsigned>(last)];
        std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
    }
    std::printf("monitor: OK=%u NO_DATA=%u TIMEOUT=%u IMPLAUSIBLE=%u DEADLINE_MISS=%u last=%s\n", counts[0], counts[1],
                counts[2], counts[3], counts[4], adas::obstacle_score::to_string(last));
    const bool ok = counts[0] >= 10U && counts[3] == 0U && counts[4] == 0U && last == MonitorVerdict::kTimeout;
    return ok ? 0 : 1;
}

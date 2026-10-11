#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "ea/osal/clock.hpp"
#include "ea/osal/periodic_task.hpp"
#include "ea/osal/realtime.hpp"

namespace {

TEST(MonotonicClock, NonDecreasing) {
    const ea::osal::MonotonicClock c;
    const auto a = c.now();
    const auto b = c.now();
    EXPECT_GE(b, a);
}

void count_cb(void* ctx) {
    static_cast<std::atomic<int>*>(ctx)->fetch_add(1);
}

TEST(PeriodicTask, RunsAtPeriod) {
    std::atomic<int> count{0};
    ea::osal::PeriodicTask task;
    const ea::osal::TaskConfig cfg{"ea_test", 2 * ea::osal::kNsPerMs, 0, 0U};
    ASSERT_TRUE(task.start(cfg, &count_cb, &count).ok());
    EXPECT_EQ(task.start(cfg, &count_cb, &count).error(), ea::ErrorCode::kAlreadyInitialized);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    task.stop();
    EXPECT_FALSE(task.running());
    EXPECT_GE(count.load(), 10);  // generous bound for loaded CI hosts
    EXPECT_EQ(static_cast<std::uint64_t>(count.load()), task.activations());
}

TEST(PeriodicTask, RejectsInvalidConfig) {
    ea::osal::PeriodicTask task;
    EXPECT_EQ(task.start({"x", 0, 0, 0U}, &count_cb, nullptr).error(), ea::ErrorCode::kInvalidArgument);
    EXPECT_EQ(task.start({"x", 1000, 0, 0U}, nullptr, nullptr).error(), ea::ErrorCode::kInvalidArgument);
}

TEST(Realtime, PrivilegeFreeCallsSucceed) {
    EXPECT_TRUE(ea::osal::set_current_thread_fifo(0).ok());
    EXPECT_TRUE(ea::osal::set_current_thread_affinity(0U).ok());
    EXPECT_TRUE(ea::osal::set_current_thread_affinity(1U).ok());
    EXPECT_EQ(ea::osal::set_current_thread_fifo(1000).error(), ea::ErrorCode::kInvalidArgument);
    ea::osal::prefault_stack(64U * 1024U);
}

}  // namespace

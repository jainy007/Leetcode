#include <gtest/gtest.h>

#include "ea/supervision/supervision.hpp"

namespace {

using ea::sup::Status;

TEST(AliveSupervision, DetectsMissingAndExcessIndications) {
    ea::sup::AliveSupervision alive({5U, 1U, 1U, 1U});
    std::uint64_t hb = 0U;
    EXPECT_EQ(alive.check(hb), Status::kOk);
    hb += 5U;
    EXPECT_EQ(alive.check(hb), Status::kOk);
    hb += 4U;  // within min margin
    EXPECT_EQ(alive.check(hb), Status::kOk);
    hb += 2U;
    EXPECT_EQ(alive.check(hb), Status::kFailed);
    hb += 9U;
    EXPECT_EQ(alive.check(hb), Status::kExpired);
    hb += 5U;
    EXPECT_EQ(alive.check(hb), Status::kExpired);  // latched
}

TEST(DeadlineSupervision, Window) {
    ea::sup::DeadlineSupervision d(100, 500);
    d.start(1000);
    EXPECT_EQ(d.stop(1300), Status::kOk);
    d.start(2000);
    EXPECT_EQ(d.stop(2600), Status::kFailed);
    EXPECT_EQ(d.stop(2700), Status::kFailed);  // stop without start
    EXPECT_EQ(d.violations(), 2U);
}

TEST(LogicalSupervision, AllowedOrder) {
    // 0 -> 1 -> 2 -> 0
    static const std::uint32_t succ[] = {0b010U, 0b100U, 0b001U};
    ea::sup::LogicalSupervision l(succ, 3U, 0U);
    EXPECT_EQ(l.reach(1U), Status::kOk);
    EXPECT_EQ(l.reach(2U), Status::kOk);
    EXPECT_EQ(l.reach(0U), Status::kOk);
    EXPECT_EQ(l.reach(2U), Status::kExpired);
}

TEST(Heartbeat, Counts) {
    ea::sup::Heartbeat hb;
    hb.beat();
    hb.beat();
    EXPECT_EQ(hb.count(), 2U);
}

}  // namespace

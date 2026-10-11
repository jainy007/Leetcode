#include <gtest/gtest.h>

#include "ea/core/result.hpp"
#include "ea/core/saturating.hpp"
#include "ea/core/static_vector.hpp"

namespace {

ea::Result<int> half(int v) {
    if (v % 2 != 0) {
        return ea::ErrorCode::kInvalidArgument;
    }
    return v / 2;
}

ea::Result<void> chain(int v) {
    EA_TRY(half(v));
    return ea::ok_result();
}

TEST(Result, ValueAndError) {
    const auto ok = half(4);
    ASSERT_TRUE(ok.ok());
    EXPECT_EQ(*ok, 2);
    const auto bad = half(3);
    EXPECT_FALSE(bad.ok());
    EXPECT_EQ(bad.error(), ea::ErrorCode::kInvalidArgument);
    EXPECT_EQ(bad.value_or(-1), -1);
    EXPECT_STREQ(ea::to_string(bad.error()), "InvalidArgument");
}

TEST(Result, TryPropagates) {
    EXPECT_TRUE(chain(2).ok());
    EXPECT_EQ(chain(1).error(), ea::ErrorCode::kInvalidArgument);
}

TEST(Result, ValueOnErrorPanics) {
    EXPECT_DEATH({ (void)half(3).value(); }, "EA_ASSERT failed");
}

TEST(Saturating, ClampsAtLimits) {
    EXPECT_EQ(ea::sat_inc<std::uint8_t>(254U), 255U);
    EXPECT_EQ(ea::sat_inc<std::uint8_t>(255U), 255U);
    EXPECT_EQ(ea::sat_add<std::uint16_t>(65000U, 1000U), 65535U);
    EXPECT_EQ(ea::sat_sub<std::uint16_t>(3U, 5U), 0U);
}

TEST(StaticVector, CapacityIsEnforced) {
    ea::StaticVector<int, 3> v;
    EXPECT_TRUE(v.push_back(1));
    EXPECT_TRUE(v.push_back(2));
    EXPECT_TRUE(v.emplace_back(3));
    EXPECT_FALSE(v.push_back(4));
    EXPECT_EQ(v.size(), 3U);
    int sum = 0;
    for (int x : v) {
        sum += x;
    }
    EXPECT_EQ(sum, 6);
    v.pop_back();
    EXPECT_EQ(v.span().size(), 2U);
    ea::StaticVector<int, 3> copy = v;
    EXPECT_EQ(copy[1], 2);
}

}  // namespace

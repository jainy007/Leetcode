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

#include <cmath>
#include <limits>

#include "ea/core/rounding.hpp"

namespace {

TEST(Rounding, MatchesLlroundIncludingTies) {
    const double specials[] = {0.0, -0.0, 0.5, -0.5, 1.5, -1.5, 2.5, -2.5, 0.49999999999999994, -0.49999999999999994,
                               4503599627370495.5, -4503599627370495.5, 1e15 + 0.5, 25000.000000000004,
                               -8000.000000000001, std::nextafter(0.5, 0.0), std::nextafter(0.5, 1.0)};
    for (const double x : specials) {
        EXPECT_EQ(ea::round_half_away(x), std::llround(x)) << x;
    }
    std::uint64_t s = 88172645463325252ULL;
    for (int i = 0; i < 2000000; ++i) {
        s ^= s << 13U;
        s ^= s >> 7U;
        s ^= s << 17U;
        // Mix of small values with fractional parts near .5 and large magnitudes.
        const double base = static_cast<double>(static_cast<std::int64_t>(s % 2000001U) - 1000000) / 2.0;
        const double x = (i % 2 == 0) ? base : base + static_cast<double>(s >> 40U) * 1e-9;
        ASSERT_EQ(ea::round_half_away(x), std::llround(x)) << x;
    }
}

}  // namespace

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <cstring>

#include "ea/gpu/cpu_ref_backend.hpp"

namespace {

using ea::gpu::DType;
using ea::gpu::OpKind;

// in(u8,4) -> norm(f32,4) -> dense(4->2) -> relu -> softmax
const ea::gpu::TensorDesc kTensors[] = {
    {"in", DType::kU8, 4U, false},    {"norm", DType::kF32, 4U, false}, {"w", DType::kF32, 8U, true},
    {"b", DType::kF32, 2U, true},     {"fc", DType::kF32, 2U, false},   {"act", DType::kF32, 2U, false},
    {"out", DType::kF32, 2U, false},
};
const ea::gpu::OpDesc kOps[] = {
    {OpKind::kNormalize, 0U, 1U, ea::gpu::kNoTensor, ea::gpu::kNoTensor, 0U, 0U, 0.5F, -1.0F},
    {OpKind::kDense, 1U, 4U, 2U, 3U, 4U, 2U, 0.0F, 0.0F},
    {OpKind::kRelu, 4U, 5U, ea::gpu::kNoTensor, ea::gpu::kNoTensor, 0U, 0U, 0.0F, 0.0F},
    {OpKind::kSoftmax, 5U, 6U, ea::gpu::kNoTensor, ea::gpu::kNoTensor, 0U, 0U, 0.0F, 0.0F},
};

template <typename T, std::size_t N>
ea::ConstByteSpan bytes(const std::array<T, N>& a) {
    return ea::ConstByteSpan(reinterpret_cast<const std::uint8_t*>(a.data()), sizeof(T) * N);
}

class CpuRef : public ::testing::Test {
protected:
    alignas(64) std::array<std::uint8_t, 1024> arena_{};
    ea::gpu::CpuRefBackend be_{arena_};
};

TEST_F(CpuRef, LifecycleIsEnforced) {
    std::array<std::uint8_t, 4> in{2U, 4U, 6U, 8U};
    EXPECT_EQ(be_.execute().error(), ea::ErrorCode::kNotInitialized);
    ASSERT_TRUE(be_.configure(kTensors, kOps).ok());
    EXPECT_EQ(be_.configure(kTensors, kOps).error(), ea::ErrorCode::kAlreadyInitialized);
    EXPECT_EQ(be_.upload(0U, bytes(in)).error(), ea::ErrorCode::kNotInitialized);  // input before seal
    EXPECT_EQ(be_.seal().error(), ea::ErrorCode::kNotInitialized);                // weights missing
    const std::array<float, 8> w{1, 0, 0, 1, 1, 0, 0, 1};
    const std::array<float, 2> b{0.0F, 0.5F};
    ASSERT_TRUE(be_.upload(2U, bytes(w)).ok());
    ASSERT_TRUE(be_.upload(3U, bytes(b)).ok());
    ASSERT_TRUE(be_.seal().ok());
    EXPECT_EQ(be_.upload(2U, bytes(w)).error(), ea::ErrorCode::kAlreadyInitialized);  // weights immutable
    const auto allocs = be_.allocation_count();

    ASSERT_TRUE(be_.upload(0U, bytes(in)).ok());
    ASSERT_TRUE(be_.execute().ok());
    std::array<float, 2> fc{};
    std::array<float, 2> out{};
    ASSERT_TRUE(be_.download(4U, ea::ByteSpan(reinterpret_cast<std::uint8_t*>(fc.data()), 8U)).ok());
    ASSERT_TRUE(be_.download(6U, ea::ByteSpan(reinterpret_cast<std::uint8_t*>(out.data()), 8U)).ok());
    // norm = {0, 1, 2, 3}; fc0 = 0 + 2 = 2; fc1 = 1 + 3 + 0.5 = 4.5
    EXPECT_FLOAT_EQ(fc[0], 2.0F);
    EXPECT_FLOAT_EQ(fc[1], 4.5F);
    const float e = std::exp(2.0F - 4.5F);
    EXPECT_NEAR(out[0], e / (1.0F + e), 1e-6);
    EXPECT_NEAR(out[0] + out[1], 1.0F, 1e-6);
    EXPECT_EQ(be_.allocation_count(), allocs);
}

TEST_F(CpuRef, RejectsInvalidGraphs) {
    ea::gpu::OpDesc bad[] = {kOps[1]};
    bad[0].in_features = 3U;
    EXPECT_EQ(be_.configure(kTensors, bad).error(), ea::ErrorCode::kInvalidArgument);
    ea::gpu::OpDesc writes_const[] = {kOps[2]};
    writes_const[0].output = 2U;
    EXPECT_EQ(ea::gpu::validate_graph(kTensors, writes_const).error(), ea::ErrorCode::kInvalidArgument);
}

TEST(CpuRefArena, TooSmall) {
    alignas(64) std::array<std::uint8_t, 128> small{};
    ea::gpu::CpuRefBackend be(small);
    EXPECT_EQ(be.configure(kTensors, kOps).error(), ea::ErrorCode::kResourceExhausted);
}

}  // namespace

namespace {

// The optimized dense loop must be bit-identical to the textbook formulation.
TEST(CpuRefDense, BitIdenticalToReferenceOrder) {
    using ea::gpu::DType;
    using ea::gpu::OpKind;
    constexpr std::uint32_t kIn = 96U;
    constexpr std::uint32_t kOut = 40U;
    const ea::gpu::TensorDesc tensors[] = {
        {"in", DType::kU8, kIn, false}, {"norm", DType::kF32, kIn, false}, {"w", DType::kF32, kIn * kOut, true},
        {"b", DType::kF32, kOut, true}, {"out", DType::kF32, kOut, false},
    };
    const ea::gpu::OpDesc ops[] = {
        {OpKind::kNormalize, 0U, 1U, ea::gpu::kNoTensor, ea::gpu::kNoTensor, 0U, 0U, 0.0123F, -0.7F},
        {OpKind::kDense, 1U, 4U, 2U, 3U, kIn, kOut, 0.0F, 0.0F},
    };
    std::array<float, kIn * kOut> w{};
    std::array<float, kOut> b{};
    std::array<std::uint8_t, kIn> in{};
    for (std::uint32_t k = 0U; k < w.size(); ++k) {
        w[k] = static_cast<float>(static_cast<int>((k * 7919U) % 2001U) - 1000) / 777.0F;
    }
    for (std::uint32_t k = 0U; k < kOut; ++k) {
        b[k] = static_cast<float>(k) * 0.37F - 3.0F;
    }
    for (std::uint32_t k = 0U; k < kIn; ++k) {
        in[k] = static_cast<std::uint8_t>(k * 13U);
    }
    alignas(64) static std::array<std::uint8_t, 32768> arena{};
    ea::gpu::CpuRefBackend be(arena);
    ASSERT_TRUE(be.configure(tensors, ops).ok());
    ASSERT_TRUE(be.upload(2U, ea::ConstByteSpan(reinterpret_cast<const std::uint8_t*>(w.data()), sizeof(w))).ok());
    ASSERT_TRUE(be.upload(3U, ea::ConstByteSpan(reinterpret_cast<const std::uint8_t*>(b.data()), sizeof(b))).ok());
    ASSERT_TRUE(be.seal().ok());
    ASSERT_TRUE(be.upload(0U, ea::ConstByteSpan(in.data(), in.size())).ok());
    ASSERT_TRUE(be.execute().ok());
    std::array<float, kOut> got{};
    ASSERT_TRUE(be.download(4U, ea::ByteSpan(reinterpret_cast<std::uint8_t*>(got.data()), sizeof(got))).ok());

    volatile float scale = 0.0123F;  // volatile: keep the reference free of compile-time folding/contraction
    volatile float shift = -0.7F;
    for (std::uint32_t j = 0U; j < kOut; ++j) {
        float acc = b[j];
        for (std::uint32_t i = 0U; i < kIn; ++i) {
            const float x = static_cast<float>(in[i]) * scale + shift;
            const float prod = x * w[i * kOut + j];
            acc = acc + prod;
        }
        EXPECT_EQ(got[j], acc) << j;  // exact, not NEAR
    }
}

}  // namespace

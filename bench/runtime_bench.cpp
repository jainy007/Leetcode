#include <benchmark/benchmark.h>

#include <array>
#include <cstdint>
#include <vector>

#include "ea/codec/bits.hpp"
#include "ea/core/crc.hpp"
#include "ea/e2e/p4.hpp"
#include "ea/gpu/cpu_ref_backend.hpp"
#include "ea/ipc/seqlock.hpp"
#include "ea/ipc/spsc_ring.hpp"

namespace {

using ea::codec::ByteOrder;

template <std::size_t N>
std::array<std::uint8_t, N> pattern() {
    std::array<std::uint8_t, N> a{};
    for (std::size_t i = 0; i < N; ++i) {
        a[i] = static_cast<std::uint8_t>(i * 37U + 11U);
    }
    return a;
}

void BM_ExtractLE16(benchmark::State& st) {
    const auto d = pattern<8>();
    for (auto _ : st) {
        benchmark::DoNotOptimize(ea::codec::extract(d, ByteOrder::kLittleEndian, 4U, 16U));
    }
}
BENCHMARK(BM_ExtractLE16);

void BM_ExtractBE16(benchmark::State& st) {
    const auto d = pattern<8>();
    for (auto _ : st) {
        benchmark::DoNotOptimize(ea::codec::extract(d, ByteOrder::kBigEndian, 15U, 16U));
    }
}
BENCHMARK(BM_ExtractBE16);

void BM_ExtractBE32Unaligned(benchmark::State& st) {
    const auto d = pattern<8>();
    for (auto _ : st) {
        benchmark::DoNotOptimize(ea::codec::extract(d, ByteOrder::kBigEndian, 3U, 32U));
    }
}
BENCHMARK(BM_ExtractBE32Unaligned);

void BM_InsertBE16(benchmark::State& st) {
    auto d = pattern<8>();
    std::uint64_t v = 0;
    for (auto _ : st) {
        benchmark::DoNotOptimize(ea::codec::insert(d, ByteOrder::kBigEndian, 15U, 16U, (v++) & 0xFFFFU));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_InsertBE16);

void BM_Crc32P4(benchmark::State& st) {
    std::array<std::uint8_t, 4096> d{};
    for (std::size_t i = 0; i < d.size(); ++i) {
        d[i] = static_cast<std::uint8_t>(i);
    }
    const auto n = static_cast<std::size_t>(st.range(0));
    for (auto _ : st) {
        benchmark::DoNotOptimize(ea::crc::crc32_p4(ea::ConstByteSpan(d.data(), n)));
    }
    st.SetBytesProcessed(static_cast<std::int64_t>(st.iterations()) * st.range(0));
}
BENCHMARK(BM_Crc32P4)->Arg(18)->Arg(256)->Arg(4096);

void BM_Crc8J1850_8B(benchmark::State& st) {
    const auto d = pattern<8>();
    for (auto _ : st) {
        benchmark::DoNotOptimize(ea::crc::crc8_j1850(d));
    }
}
BENCHMARK(BM_Crc8J1850_8B);

void BM_E2eP4Check18B(benchmark::State& st) {
    constexpr ea::e2e::P4Config cfg{0x0A0B0C0DU, 0U, 18U, 18U, 0xFFFFU};
    ea::e2e::P4Protector tx(cfg);
    ea::e2e::P4Checker rx(cfg);
    std::array<std::uint8_t, 18> p{};
    for (auto _ : st) {
        (void)tx.protect(p);
        benchmark::DoNotOptimize(rx.check(p));
    }
}
BENCHMARK(BM_E2eP4Check18B);

struct Sample {
    std::uint64_t a, b, c, d;
};

void BM_SeqlockStoreLoad(benchmark::State& st) {
    static ea::ipc::Seqlock<Sample> lock;
    std::uint64_t i = 0;
    for (auto _ : st) {
        lock.store(Sample{i, i, i, i});
        benchmark::DoNotOptimize(lock.load());
        ++i;
    }
}
BENCHMARK(BM_SeqlockStoreLoad);

void BM_SpscPushPop(benchmark::State& st) {
    static ea::ipc::SpscRing<std::uint64_t, 1024> ring;
    std::uint64_t v = 0;
    for (auto _ : st) {
        (void)ring.push(v);
        benchmark::DoNotOptimize(ring.pop(v));
    }
}
BENCHMARK(BM_SpscPushPop);

void BM_CpuRefDense(benchmark::State& st) {
    using ea::gpu::DType;
    using ea::gpu::OpKind;
    const auto in_f = static_cast<std::uint32_t>(st.range(0));
    const auto out_f = static_cast<std::uint32_t>(st.range(1));
    const ea::gpu::TensorDesc tensors[] = {
        {"in", DType::kU8, in_f, false},      {"norm", DType::kF32, in_f, false},
        {"w", DType::kF32, in_f * out_f, true}, {"b", DType::kF32, out_f, true},
        {"out", DType::kF32, out_f, false},
    };
    const ea::gpu::OpDesc ops[] = {
        {OpKind::kNormalize, 0U, 1U, ea::gpu::kNoTensor, ea::gpu::kNoTensor, 0U, 0U, 1.0F / 255.0F, 0.0F},
        {OpKind::kDense, 1U, 4U, 2U, 3U, in_f, out_f, 0.0F, 0.0F},
    };
    alignas(64) static std::array<std::uint8_t, 1U << 22U> arena{};
    ea::gpu::CpuRefBackend be(arena);
    (void)be.configure(tensors, ops);
    std::vector<float> w(in_f * out_f, 0.01F);
    std::vector<float> b(out_f, 0.1F);
    (void)be.upload(2U, ea::ConstByteSpan(reinterpret_cast<const std::uint8_t*>(w.data()), w.size() * 4U));
    (void)be.upload(3U, ea::ConstByteSpan(reinterpret_cast<const std::uint8_t*>(b.data()), b.size() * 4U));
    (void)be.seal();
    std::vector<std::uint8_t> in(in_f, 7U);
    for (auto _ : st) {
        (void)be.upload(0U, ea::ConstByteSpan(in.data(), in.size()));
        benchmark::DoNotOptimize(be.execute());
    }
    st.SetItemsProcessed(static_cast<std::int64_t>(st.iterations()) * in_f * out_f);
}
BENCHMARK(BM_CpuRefDense)->Args({64, 4})->Args({256, 64})->Args({1024, 256});

}  // namespace

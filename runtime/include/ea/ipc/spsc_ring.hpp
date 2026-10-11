// Wait-free single-producer/single-consumer ring buffer (rules CPU-011, MP-003).
// - Capacity N must be a power of two; one slot is never wasted (indices are free-running).
// - Head/tail live on separate cache lines to avoid false sharing.
// - Address-free: may be placed in shared memory between processes (T trivially copyable,
//   atomics always lock-free).
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ea::ipc {

inline constexpr std::size_t kCacheLine = 64U;

template <typename T, std::size_t N>
class SpscRing {
    static_assert(N >= 2U && (N & (N - 1U)) == 0U, "capacity must be a power of two");
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
    static_assert(std::atomic<std::uint32_t>::is_always_lock_free, "requires lock-free 32-bit atomics");

public:
    static constexpr std::size_t capacity() noexcept { return N; }

    // Producer side. Returns false if full.
    bool push(const T& v) noexcept {
        const std::uint32_t head = head_.load(std::memory_order_relaxed);
        const std::uint32_t tail = tail_.load(std::memory_order_acquire);
        if (static_cast<std::uint32_t>(head - tail) == N) {
            return false;
        }
        slots_[head & kMask] = v;
        head_.store(head + 1U, std::memory_order_release);
        return true;
    }

    // Consumer side. Returns false if empty.
    bool pop(T& out) noexcept {
        const std::uint32_t tail = tail_.load(std::memory_order_relaxed);
        const std::uint32_t head = head_.load(std::memory_order_acquire);
        if (head == tail) {
            return false;
        }
        out = slots_[tail & kMask];
        tail_.store(tail + 1U, std::memory_order_release);
        return true;
    }

    std::size_t size() const noexcept {
        return static_cast<std::uint32_t>(head_.load(std::memory_order_acquire) -
                                          tail_.load(std::memory_order_acquire));
    }
    bool empty() const noexcept { return size() == 0U; }

private:
    static constexpr std::uint32_t kMask = static_cast<std::uint32_t>(N - 1U);

    alignas(kCacheLine) std::atomic<std::uint32_t> head_{0U};
    alignas(kCacheLine) std::atomic<std::uint32_t> tail_{0U};
    alignas(kCacheLine) std::array<T, N> slots_{};
};

}  // namespace ea::ipc

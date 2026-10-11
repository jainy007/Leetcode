// Single-writer seqlock for "latest value" exchange (rule MP-004).
// The payload is stored as atomic words, so concurrent reads are data-race free under the
// C++ memory model (TSan clean, no fences). Ordering: the writer publishes the odd sequence
// before release-storing data words; a reader that acquire-loads any new word therefore
// observes a changed sequence on its re-check and retries.
// Address-free: may be placed in shared memory.
#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "ea/core/result.hpp"

namespace ea::ipc {

template <typename T>
class Seqlock {
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
    static_assert(std::atomic<std::uint64_t>::is_always_lock_free, "requires lock-free 64-bit atomics");

public:
    // Writer only (single writer).
    void store(const T& value) noexcept {
        std::array<std::uint64_t, kWords> words{};
        std::memcpy(words.data(), &value, sizeof(T));
        const std::uint64_t seq = seq_.load(std::memory_order_relaxed);
        seq_.store(seq + 1U, std::memory_order_relaxed);  // odd: write in progress
        for (std::size_t i = 0U; i < kWords; ++i) {
            data_[i].store(words[i], std::memory_order_release);
        }
        seq_.store(seq + 2U, std::memory_order_release);  // even: stable
    }

    // Reader: retries at most `max_retries` times while a write is in progress.
    // Returns kTimeout if no consistent snapshot was obtained, kNotInitialized if never written.
    Result<T> load(std::uint32_t max_retries = 64U) const noexcept {
        for (std::uint32_t attempt = 0U; attempt <= max_retries; ++attempt) {
            const std::uint64_t s1 = seq_.load(std::memory_order_acquire);
            if (s1 == 0U) {
                return ErrorCode::kNotInitialized;
            }
            if ((s1 & 1U) != 0U) {
                continue;
            }
            std::array<std::uint64_t, kWords> words{};
            for (std::size_t i = 0U; i < kWords; ++i) {
                words[i] = data_[i].load(std::memory_order_acquire);
            }
            if (seq_.load(std::memory_order_relaxed) == s1) {
                T out;
                std::memcpy(&out, words.data(), sizeof(T));
                return out;
            }
        }
        return ErrorCode::kTimeout;
    }

    // Number of completed writes.
    std::uint64_t version() const noexcept { return seq_.load(std::memory_order_acquire) / 2U; }

private:
    static constexpr std::size_t kWords = (sizeof(T) + sizeof(std::uint64_t) - 1U) / sizeof(std::uint64_t);

    std::atomic<std::uint64_t> seq_{0U};
    std::array<std::atomic<std::uint64_t>, kWords> data_{};
};

}  // namespace ea::ipc

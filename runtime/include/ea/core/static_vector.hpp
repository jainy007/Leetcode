// StaticVector<T, N>: fixed-capacity vector with inline storage, no heap (rule CPU-004).
#pragma once

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "ea/core/assert.hpp"
#include "ea/core/span.hpp"

namespace ea {

template <typename T, std::size_t N>
class StaticVector {
    static_assert(N > 0U, "capacity must be > 0");

public:
    StaticVector() noexcept = default;
    StaticVector(const StaticVector& other) {
        for (const T& v : other) {
            (void)push_back(v);
        }
    }
    StaticVector& operator=(const StaticVector& other) {
        if (this != &other) {
            clear();
            for (const T& v : other) {
                (void)push_back(v);
            }
        }
        return *this;
    }
    ~StaticVector() { clear(); }

    static constexpr std::size_t capacity() noexcept { return N; }
    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0U; }
    bool full() const noexcept { return size_ == N; }

    // Returns false (and leaves the vector unchanged) when full.
    [[nodiscard]] bool push_back(const T& v) {
        if (full()) {
            return false;
        }
        ::new (static_cast<void*>(slot(size_))) T(v);
        ++size_;
        return true;
    }

    template <typename... Args>
    [[nodiscard]] bool emplace_back(Args&&... args) {
        if (full()) {
            return false;
        }
        ::new (static_cast<void*>(slot(size_))) T(std::forward<Args>(args)...);
        ++size_;
        return true;
    }

    void pop_back() {
        EA_ASSERT(!empty());
        --size_;
        slot(size_)->~T();
    }

    void clear() noexcept {
        while (size_ > 0U) {
            --size_;
            slot(size_)->~T();
        }
    }

    T& operator[](std::size_t i) {
        EA_ASSERT(i < size_);
        return *slot(i);
    }
    const T& operator[](std::size_t i) const {
        EA_ASSERT(i < size_);
        return *slot(i);
    }

    T* begin() noexcept { return slot(0U); }
    T* end() noexcept { return slot(0U) + size_; }
    const T* begin() const noexcept { return slot(0U); }
    const T* end() const noexcept { return slot(0U) + size_; }

    Span<T> span() noexcept { return Span<T>(begin(), size_); }
    Span<const T> span() const noexcept { return Span<const T>(begin(), size_); }

private:
    T* slot(std::size_t i) noexcept { return std::launder(reinterpret_cast<T*>(&storage_[i])); }  // NOLINT
    const T* slot(std::size_t i) const noexcept {
        return std::launder(reinterpret_cast<const T*>(&storage_[i]));  // NOLINT
    }

    std::aligned_storage_t<sizeof(T), alignof(T)> storage_[N];
    std::size_t size_ = 0U;
};

}  // namespace ea

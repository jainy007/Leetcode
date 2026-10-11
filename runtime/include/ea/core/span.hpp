// Span<T>: non-owning view over contiguous memory (std::span is C++20; we target C++17).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "ea/core/assert.hpp"

namespace ea {

template <typename T>
class Span {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;

    constexpr Span() noexcept = default;
    constexpr Span(T* data, std::size_t size) noexcept : data_(data), size_(size) {}

    template <std::size_t N>
    constexpr Span(T (&arr)[N]) noexcept : data_(arr), size_(N) {}  // NOLINT

    template <typename U, std::size_t N, typename = std::enable_if_t<std::is_convertible<U (*)[], T (*)[]>::value>>
    constexpr Span(std::array<U, N>& arr) noexcept : data_(arr.data()), size_(N) {}  // NOLINT

    template <typename U, std::size_t N,
              typename = std::enable_if_t<std::is_convertible<const U (*)[], T (*)[]>::value>>
    constexpr Span(const std::array<U, N>& arr) noexcept : data_(arr.data()), size_(N) {}  // NOLINT

    template <typename U, typename = std::enable_if_t<std::is_convertible<U (*)[], T (*)[]>::value>>
    constexpr Span(const Span<U>& other) noexcept : data_(other.data()), size_(other.size()) {}  // NOLINT

    constexpr T* data() const noexcept { return data_; }
    constexpr std::size_t size() const noexcept { return size_; }
    constexpr bool empty() const noexcept { return size_ == 0U; }
    constexpr T* begin() const noexcept { return data_; }
    constexpr T* end() const noexcept { return data_ + size_; }

    T& operator[](std::size_t i) const {
        EA_ASSERT(i < size_);
        return data_[i];
    }

    Span subspan(std::size_t offset, std::size_t count) const {
        EA_ASSERT(offset <= size_ && count <= size_ - offset);
        return Span(data_ + offset, count);
    }
    Span subspan(std::size_t offset) const {
        EA_ASSERT(offset <= size_);
        return Span(data_ + offset, size_ - offset);
    }

private:
    T* data_ = nullptr;
    std::size_t size_ = 0U;
};

using ByteSpan = Span<std::uint8_t>;
using ConstByteSpan = Span<const std::uint8_t>;

}  // namespace ea

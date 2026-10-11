// Result<T>: value-or-error return type for code built without exceptions (rule CPU-001).
#pragma once

#include <cstdint>
#include <optional>
#include <type_traits>
#include <utility>

#include "ea/core/assert.hpp"

namespace ea {

enum class ErrorCode : std::uint16_t {
    kOk = 0,
    kInvalidArgument,
    kOutOfRange,
    kBufferTooSmall,
    kLengthMismatch,
    kCrcMismatch,
    kSequenceError,
    kProtocolError,
    kTimeout,
    kDeadlineMiss,
    kResourceExhausted,
    kNotInitialized,
    kAlreadyInitialized,
    kNotSupported,
    kOsError,
    kIoError,
    kIntegrityError,
    kPlausibilityError,
};

const char* to_string(ErrorCode code) noexcept;

template <typename T>
class [[nodiscard]] Result {
    static_assert(!std::is_reference<T>::value, "Result<T&> is not supported");

public:
    // Implicit on purpose: `return value;` and `return ErrorCode::kX;` both read naturally.
    Result(const T& value) : value_(value), error_(ErrorCode::kOk) {}               // NOLINT
    Result(T&& value) : value_(std::move(value)), error_(ErrorCode::kOk) {}         // NOLINT
    Result(ErrorCode error) : value_(std::nullopt), error_(error) {                 // NOLINT
        EA_ASSERT(error != ErrorCode::kOk);
    }

    bool ok() const noexcept { return error_ == ErrorCode::kOk; }
    explicit operator bool() const noexcept { return ok(); }
    ErrorCode error() const noexcept { return error_; }

    const T& value() const& {
        EA_ASSERT(ok());
        return *value_;
    }
    T& value() & {
        EA_ASSERT(ok());
        return *value_;
    }
    T&& value() && {
        EA_ASSERT(ok());
        return std::move(*value_);
    }
    const T& operator*() const& { return value(); }
    T& operator*() & { return value(); }
    const T* operator->() const { return &value(); }
    T* operator->() { return &value(); }

    T value_or(const T& fallback) const { return ok() ? *value_ : fallback; }

private:
    std::optional<T> value_;
    ErrorCode error_;
};

template <>
class [[nodiscard]] Result<void> {
public:
    Result() noexcept : error_(ErrorCode::kOk) {}
    Result(ErrorCode error) noexcept : error_(error) {}  // NOLINT

    bool ok() const noexcept { return error_ == ErrorCode::kOk; }
    explicit operator bool() const noexcept { return ok(); }
    ErrorCode error() const noexcept { return error_; }

private:
    ErrorCode error_;
};

inline Result<void> ok_result() noexcept {
    return Result<void>{};
}

}  // namespace ea

// Propagate an error from an expression returning Result<...>.
#define EA_TRY(expr)                        \
    do {                                    \
        const auto ea_try_result_ = (expr); \
        if (!ea_try_result_.ok()) {         \
            return ea_try_result_.error();  \
        }                                   \
    } while (false)

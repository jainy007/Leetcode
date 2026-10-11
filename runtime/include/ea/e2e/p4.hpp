// AUTOSAR E2E Profile 4 (PRS_E2E): 12-byte header placed at `offset_bytes` in the payload.
//   Length  (uint16, BE)  total payload length in bytes
//   Counter (uint16, BE)  incremented per sent message, wraps 0xFFFF -> 0
//   DataID  (uint32, BE)  unique id of the protected data element
//   CRC     (uint32, BE)  CRC32P4 over payload excluding the CRC field
#pragma once

#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/core/span.hpp"

namespace ea::e2e {

inline constexpr std::size_t kP4HeaderSize = 12U;

struct P4Config {
    std::uint32_t data_id;
    std::uint16_t offset_bytes;
    std::uint16_t min_length;
    std::uint16_t max_length;
    std::uint16_t max_delta_counter;
};

enum class P4Status : std::uint8_t {
    kOk,            // counter delta == 1
    kOkSomeLost,    // 1 < delta <= max_delta_counter
    kRepeated,      // delta == 0
    kWrongSequence, // delta > max_delta_counter
    kError,         // length / data id / CRC mismatch
    kNoNewData,     // nothing received
};

const char* to_string(P4Status s) noexcept;

inline bool is_usable(P4Status s) noexcept {
    return s == P4Status::kOk || s == P4Status::kOkSomeLost;
}

class P4Protector {
public:
    explicit P4Protector(const P4Config& cfg) noexcept : cfg_(cfg) {}
    Result<void> protect(ByteSpan payload) noexcept;
    std::uint16_t counter() const noexcept { return counter_; }

private:
    P4Config cfg_;
    std::uint16_t counter_ = 0U;
};

class P4Checker {
public:
    explicit P4Checker(const P4Config& cfg) noexcept : cfg_(cfg) {}
    // An empty payload means "no data received this cycle".
    P4Status check(ConstByteSpan payload) noexcept;

private:
    P4Config cfg_;
    std::uint16_t last_counter_ = 0xFFFFU;  // per PRS: first message with counter 0 yields delta 1
};

// CRC over the payload without the 4-byte CRC field (exposed for test-frame construction).
std::uint32_t p4_crc(ConstByteSpan payload, std::uint16_t offset_bytes) noexcept;

}  // namespace ea::e2e

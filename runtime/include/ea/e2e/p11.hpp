// E2E protection for classic CAN, modelled on AUTOSAR E2E Profile 11 (DataIDMode BOTH):
//   CRC8 SAE-J1850 (start 0xFF, xorout 0xFF) over DataID low byte, DataID high byte, then all
//   payload bytes except the CRC byte; 4-bit alive counter 0..14 (0xF is reserved and invalid).
// Positions are configurable: the CRC occupies one whole byte, the counter one nibble.
#pragma once

#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/core/span.hpp"
#include "ea/e2e/p4.hpp"  // P4Status is reused as the generic E2E check status

namespace ea::e2e {

struct P11Config {
    std::uint16_t data_id;
    std::uint8_t crc_byte;          // byte index of the CRC
    std::uint8_t counter_bit;       // bit index of the counter LSB; must be 0 or 4 within its byte
    std::uint8_t length;            // expected payload length in bytes (1..8 for classic CAN)
    std::uint8_t max_delta_counter; // 1..14
};

inline constexpr std::uint8_t kP11CounterModulo = 15U;

// CRC per the profile; exposed for test-frame construction and independent checks.
std::uint8_t p11_crc(const P11Config& cfg, ConstByteSpan payload) noexcept;
bool p11_config_valid(const P11Config& cfg) noexcept;

class P11Protector {
public:
    explicit P11Protector(const P11Config& cfg) noexcept : cfg_(cfg) {}
    // Writes counter and CRC into an already encoded payload, then advances the counter.
    Result<void> protect(ByteSpan payload) noexcept;
    std::uint8_t counter() const noexcept { return counter_; }

private:
    P11Config cfg_;
    std::uint8_t counter_ = 0U;
};

class P11Checker {
public:
    explicit P11Checker(const P11Config& cfg) noexcept : cfg_(cfg) {}
    // Empty payload = nothing received this cycle.
    P4Status check(ConstByteSpan payload) noexcept;

private:
    P11Config cfg_;
    std::uint8_t last_counter_ = kP11CounterModulo - 1U;  // first counter 0 yields delta 1
};

}  // namespace ea::e2e

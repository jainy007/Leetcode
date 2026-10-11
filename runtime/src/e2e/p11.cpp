#include "ea/e2e/p11.hpp"

#include "ea/core/crc.hpp"

namespace ea::e2e {

namespace {

std::uint8_t read_counter(const P11Config& cfg, ConstByteSpan p) noexcept {
    return static_cast<std::uint8_t>((static_cast<std::uint32_t>(p[cfg.counter_bit / 8U]) >> (cfg.counter_bit % 8U)) &
                                     0x0FU);
}

}  // namespace

bool p11_config_valid(const P11Config& cfg) noexcept {
    const bool counter_aligned = (cfg.counter_bit % 4U) == 0U;
    const bool in_range = cfg.length >= 2U && cfg.length <= 8U && cfg.crc_byte < cfg.length &&
                          (cfg.counter_bit / 8U) < cfg.length && (cfg.counter_bit / 8U) != cfg.crc_byte;
    return counter_aligned && in_range && cfg.max_delta_counter >= 1U && cfg.max_delta_counter < kP11CounterModulo;
}

std::uint8_t p11_crc(const P11Config& cfg, ConstByteSpan payload) noexcept {
    // CRC8 SAE-J1850 computed incrementally over [id_lo, id_hi, payload \ crc byte].
    std::uint8_t c = 0xFFU;
    const auto feed = [&c](std::uint8_t b) {
        c = crc::detail::kCrc8J1850Table[static_cast<std::uint8_t>(c ^ b)];
    };
    feed(static_cast<std::uint8_t>(cfg.data_id & 0xFFU));
    feed(static_cast<std::uint8_t>(cfg.data_id >> 8U));
    for (std::size_t i = 0U; i < payload.size(); ++i) {
        if (i != cfg.crc_byte) {
            feed(payload[i]);
        }
    }
    return static_cast<std::uint8_t>(c ^ 0xFFU);
}

Result<void> P11Protector::protect(ByteSpan payload) noexcept {
    if (!p11_config_valid(cfg_) || payload.size() != cfg_.length) {
        return ErrorCode::kLengthMismatch;
    }
    const std::size_t cb = cfg_.counter_bit / 8U;
    const std::uint32_t shift = cfg_.counter_bit % 8U;
    const std::uint32_t mask = 0x0FU << shift;
    payload[cb] = static_cast<std::uint8_t>((static_cast<std::uint32_t>(payload[cb]) & ~mask) |
                                            (static_cast<std::uint32_t>(counter_) << shift));
    payload[cfg_.crc_byte] = p11_crc(cfg_, ConstByteSpan(payload));
    counter_ = static_cast<std::uint8_t>((counter_ + 1U) % kP11CounterModulo);
    return ok_result();
}

P4Status P11Checker::check(ConstByteSpan payload) noexcept {
    if (payload.empty()) {
        return P4Status::kNoNewData;
    }
    if (!p11_config_valid(cfg_) || payload.size() != cfg_.length) {
        return P4Status::kError;
    }
    const std::uint8_t counter = read_counter(cfg_, payload);
    if (counter >= kP11CounterModulo || payload[cfg_.crc_byte] != p11_crc(cfg_, payload)) {
        return P4Status::kError;
    }
    const auto delta = static_cast<std::uint8_t>((counter + kP11CounterModulo - last_counter_) % kP11CounterModulo);
    last_counter_ = counter;
    if (delta == 0U) {
        return P4Status::kRepeated;
    }
    if (delta == 1U) {
        return P4Status::kOk;
    }
    return (delta <= cfg_.max_delta_counter) ? P4Status::kOkSomeLost : P4Status::kWrongSequence;
}

}  // namespace ea::e2e

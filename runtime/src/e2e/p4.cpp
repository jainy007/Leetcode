#include "ea/e2e/p4.hpp"

#include "ea/codec/bits.hpp"
#include "ea/core/crc.hpp"

namespace ea::e2e {

namespace {

constexpr std::size_t kCrcFieldOffset = 8U;  // within the header
constexpr std::size_t kCrcFieldSize = 4U;

bool length_valid(const P4Config& cfg, std::size_t len) noexcept {
    return len >= static_cast<std::size_t>(cfg.offset_bytes) + kP4HeaderSize && len >= cfg.min_length &&
           len <= cfg.max_length && len <= 0xFFFFU;
}

}  // namespace

const char* to_string(P4Status s) noexcept {
    switch (s) {
        case P4Status::kOk:
            return "OK";
        case P4Status::kOkSomeLost:
            return "OKSOMELOST";
        case P4Status::kRepeated:
            return "REPEATED";
        case P4Status::kWrongSequence:
            return "WRONGSEQUENCE";
        case P4Status::kError:
            return "ERROR";
        case P4Status::kNoNewData:
            return "NONEWDATA";
    }
    return "UNKNOWN";
}

std::uint32_t p4_crc(ConstByteSpan payload, std::uint16_t offset_bytes) noexcept {
    const std::size_t crc_pos = static_cast<std::size_t>(offset_bytes) + kCrcFieldOffset;
    crc::Crc32P4 c;
    c.update(payload.subspan(0U, crc_pos));
    c.update(payload.subspan(crc_pos + kCrcFieldSize));
    return c.final();
}

Result<void> P4Protector::protect(ByteSpan payload) noexcept {
    if (!length_valid(cfg_, payload.size())) {
        return ErrorCode::kLengthMismatch;
    }
    std::uint8_t* h = payload.data() + cfg_.offset_bytes;
    codec::store_be16(h, static_cast<std::uint16_t>(payload.size()));
    codec::store_be16(h + 2U, counter_);
    codec::store_be32(h + 4U, cfg_.data_id);
    codec::store_be32(h + kCrcFieldOffset, p4_crc(ConstByteSpan(payload), cfg_.offset_bytes));
    counter_ = static_cast<std::uint16_t>(counter_ + 1U);
    return ok_result();
}

P4Status P4Checker::check(ConstByteSpan payload) noexcept {
    if (payload.empty()) {
        return P4Status::kNoNewData;
    }
    if (!length_valid(cfg_, payload.size())) {
        return P4Status::kError;
    }
    const std::uint8_t* h = payload.data() + cfg_.offset_bytes;
    const std::uint16_t length = codec::load_be16(h);
    const std::uint16_t counter = codec::load_be16(h + 2U);
    const std::uint32_t data_id = codec::load_be32(h + 4U);
    const std::uint32_t crc = codec::load_be32(h + kCrcFieldOffset);
    if (length != payload.size() || data_id != cfg_.data_id || crc != p4_crc(payload, cfg_.offset_bytes)) {
        return P4Status::kError;
    }
    const std::uint16_t delta = static_cast<std::uint16_t>(counter - last_counter_);
    last_counter_ = counter;
    if (delta == 0U) {
        return P4Status::kRepeated;
    }
    if (delta == 1U) {
        return P4Status::kOk;
    }
    if (delta <= cfg_.max_delta_counter) {
        return P4Status::kOkSomeLost;
    }
    return P4Status::kWrongSequence;
}

}  // namespace ea::e2e

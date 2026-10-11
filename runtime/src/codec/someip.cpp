#include "ea/codec/someip.hpp"

#include "ea/codec/bits.hpp"

namespace ea::someip {

Result<ConstByteSpan> parse(ConstByteSpan frame, const Expected& expected) noexcept {
    if (frame.size() < kHeaderSize) {
        return ErrorCode::kLengthMismatch;
    }
    const std::uint8_t* p = frame.data();
    const std::uint32_t length = codec::load_be32(p + 4U);
    // Length counts from client id (byte 8) to the end of the payload.
    if (length < 8U || static_cast<std::size_t>(length) + 8U != frame.size()) {
        return ErrorCode::kLengthMismatch;
    }
    if (codec::load_be16(p) != expected.service_id || codec::load_be16(p + 2U) != expected.method_id) {
        return ErrorCode::kProtocolError;
    }
    if (p[12] != kProtocolVersion || p[13] != expected.interface_version ||
        p[14] != static_cast<std::uint8_t>(expected.message_type) || p[15] != 0U) {
        return ErrorCode::kProtocolError;
    }
    return frame.subspan(kHeaderSize);
}

Result<void> write_header(ByteSpan frame, const Expected& id, std::uint16_t session_id,
                          std::size_t payload_size) noexcept {
    if (frame.size() < kHeaderSize + payload_size) {
        return ErrorCode::kBufferTooSmall;
    }
    std::uint8_t* p = frame.data();
    codec::store_be16(p, id.service_id);
    codec::store_be16(p + 2U, id.method_id);
    codec::store_be32(p + 4U, static_cast<std::uint32_t>(payload_size + 8U));
    codec::store_be16(p + 8U, 0U);
    codec::store_be16(p + 10U, session_id);
    p[12] = kProtocolVersion;
    p[13] = id.interface_version;
    p[14] = static_cast<std::uint8_t>(id.message_type);
    p[15] = 0U;
    return ok_result();
}

}  // namespace ea::someip

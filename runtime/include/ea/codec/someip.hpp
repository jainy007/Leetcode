// SOME/IP header (AUTOSAR PRS_SOMEIP), 16 bytes, big endian.
//   [0..1] service id  [2..3] method/event id  [4..7] length (bytes after this field)
//   [8..9] client id   [10..11] session id     [12] protocol version (0x01)
//   [13] interface version  [14] message type  [15] return code
#pragma once

#include <cstdint>

#include "ea/core/result.hpp"
#include "ea/core/span.hpp"

namespace ea::someip {

inline constexpr std::size_t kHeaderSize = 16U;
inline constexpr std::uint8_t kProtocolVersion = 0x01U;

enum class MessageType : std::uint8_t {
    kRequest = 0x00U,
    kRequestNoReturn = 0x01U,
    kNotification = 0x02U,
    kResponse = 0x80U,
    kError = 0x81U,
};

struct Header {
    std::uint16_t service_id;
    std::uint16_t method_id;
    std::uint32_t length;
    std::uint16_t client_id;
    std::uint16_t session_id;
    std::uint8_t protocol_version;
    std::uint8_t interface_version;
    MessageType message_type;
    std::uint8_t return_code;
};

struct Expected {
    std::uint16_t service_id;
    std::uint16_t method_id;
    std::uint8_t interface_version;
    MessageType message_type;
};

// Parses and validates a header against the expected identity; returns the payload view.
Result<ConstByteSpan> parse(ConstByteSpan frame, const Expected& expected) noexcept;

// Writes a header for `payload_size` bytes of payload into frame[0..16).
Result<void> write_header(ByteSpan frame, const Expected& id, std::uint16_t session_id,
                          std::size_t payload_size) noexcept;

}  // namespace ea::someip

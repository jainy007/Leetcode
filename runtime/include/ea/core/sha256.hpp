// SHA-256 (FIPS 180-4). Used to verify integrity of model artefacts (weights, engines) at init
// against hashes recorded in the model (rule GPU-006).
#pragma once

#include <array>
#include <cstdint>

#include "ea/core/span.hpp"

namespace ea {

using Sha256Digest = std::array<std::uint8_t, 32>;

class Sha256 {
public:
    Sha256() noexcept;
    void update(ConstByteSpan data) noexcept;
    Sha256Digest final() noexcept;

    static Sha256Digest digest(ConstByteSpan data) noexcept;

private:
    void compress(const std::uint8_t* block) noexcept;

    std::array<std::uint32_t, 8> state_;
    std::array<std::uint8_t, 64> buffer_{};
    std::uint64_t total_len_ = 0U;
    std::size_t buffer_len_ = 0U;
};

// Parses a 64-character lowercase/uppercase hex string. Returns false on malformed input.
bool parse_sha256_hex(const char* hex, Sha256Digest& out) noexcept;

}  // namespace ea

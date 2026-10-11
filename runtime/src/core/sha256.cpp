#include "ea/core/sha256.hpp"

#include <cstring>

namespace ea {

namespace {

constexpr std::array<std::uint32_t, 64> kK = {
    0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U, 0x3956c25bU, 0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U,
    0xd807aa98U, 0x12835b01U, 0x243185beU, 0x550c7dc3U, 0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U, 0xc19bf174U,
    0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU, 0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU,
    0x983e5152U, 0xa831c66dU, 0xb00327c8U, 0xbf597fc7U, 0xc6e00bf3U, 0xd5a79147U, 0x06ca6351U, 0x14292967U,
    0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU, 0x53380d13U, 0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
    0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U, 0xd192e819U, 0xd6990624U, 0xf40e3585U, 0x106aa070U,
    0x19a4c116U, 0x1e376c08U, 0x2748774cU, 0x34b0bcb5U, 0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU, 0x682e6ff3U,
    0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U, 0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U,
};

constexpr std::uint32_t rotr(std::uint32_t x, std::uint32_t n) noexcept {
    return (x >> n) | (x << (32U - n));
}

int hex_value(char c) noexcept {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

}  // namespace

Sha256::Sha256() noexcept
    : state_{0x6a09e667U, 0xbb67ae85U, 0x3c6ef372U, 0xa54ff53aU, 0x510e527fU, 0x9b05688cU, 0x1f83d9abU, 0x5be0cd19U} {}

void Sha256::compress(const std::uint8_t* block) noexcept {
    std::array<std::uint32_t, 64> w{};
    for (std::size_t i = 0U; i < 16U; ++i) {
        w[i] = (static_cast<std::uint32_t>(block[i * 4U]) << 24U) |
               (static_cast<std::uint32_t>(block[i * 4U + 1U]) << 16U) |
               (static_cast<std::uint32_t>(block[i * 4U + 2U]) << 8U) | static_cast<std::uint32_t>(block[i * 4U + 3U]);
    }
    for (std::size_t i = 16U; i < 64U; ++i) {
        const std::uint32_t s0 = rotr(w[i - 15U], 7U) ^ rotr(w[i - 15U], 18U) ^ (w[i - 15U] >> 3U);
        const std::uint32_t s1 = rotr(w[i - 2U], 17U) ^ rotr(w[i - 2U], 19U) ^ (w[i - 2U] >> 10U);
        w[i] = w[i - 16U] + s0 + w[i - 7U] + s1;
    }
    std::uint32_t a = state_[0];
    std::uint32_t b = state_[1];
    std::uint32_t c = state_[2];
    std::uint32_t d = state_[3];
    std::uint32_t e = state_[4];
    std::uint32_t f = state_[5];
    std::uint32_t g = state_[6];
    std::uint32_t h = state_[7];
    for (std::size_t i = 0U; i < 64U; ++i) {
        const std::uint32_t s1 = rotr(e, 6U) ^ rotr(e, 11U) ^ rotr(e, 25U);
        const std::uint32_t ch = (e & f) ^ (~e & g);
        const std::uint32_t t1 = h + s1 + ch + kK[i] + w[i];
        const std::uint32_t s0 = rotr(a, 2U) ^ rotr(a, 13U) ^ rotr(a, 22U);
        const std::uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        const std::uint32_t t2 = s0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    state_[4] += e;
    state_[5] += f;
    state_[6] += g;
    state_[7] += h;
}

void Sha256::update(ConstByteSpan data) noexcept {
    total_len_ += data.size();
    std::size_t i = 0U;
    while (i < data.size()) {
        const std::size_t take = (64U - buffer_len_) < (data.size() - i) ? (64U - buffer_len_) : (data.size() - i);
        std::memcpy(&buffer_[buffer_len_], data.data() + i, take);
        buffer_len_ += take;
        i += take;
        if (buffer_len_ == 64U) {
            compress(buffer_.data());
            buffer_len_ = 0U;
        }
    }
}

Sha256Digest Sha256::final() noexcept {
    const std::uint64_t bit_len = total_len_ * 8U;
    const std::uint8_t pad_start = 0x80U;
    update(ConstByteSpan(&pad_start, 1U));
    const std::uint8_t zero = 0U;
    while (buffer_len_ != 56U) {
        update(ConstByteSpan(&zero, 1U));
    }
    std::array<std::uint8_t, 8> len_be{};
    for (std::size_t i = 0U; i < 8U; ++i) {
        len_be[i] = static_cast<std::uint8_t>(bit_len >> (56U - 8U * i));
    }
    update(ConstByteSpan(len_be.data(), len_be.size()));
    Sha256Digest out{};
    for (std::size_t i = 0U; i < 8U; ++i) {
        out[i * 4U] = static_cast<std::uint8_t>(state_[i] >> 24U);
        out[i * 4U + 1U] = static_cast<std::uint8_t>(state_[i] >> 16U);
        out[i * 4U + 2U] = static_cast<std::uint8_t>(state_[i] >> 8U);
        out[i * 4U + 3U] = static_cast<std::uint8_t>(state_[i]);
    }
    return out;
}

Sha256Digest Sha256::digest(ConstByteSpan data) noexcept {
    Sha256 h;
    h.update(data);
    return h.final();
}

bool parse_sha256_hex(const char* hex, Sha256Digest& out) noexcept {
    if (hex == nullptr) {
        return false;
    }
    for (std::size_t i = 0U; i < 32U; ++i) {
        const int hi = hex_value(hex[2U * i]);
        if (hi < 0) {
            return false;
        }
        const int lo = hex_value(hex[2U * i + 1U]);
        if (lo < 0) {
            return false;
        }
        out[i] = static_cast<std::uint8_t>((hi << 4) | lo);
    }
    return hex[64] == '\0';
}

}  // namespace ea

// Private temporary directory for SIL artefacts (e.g. weight files a test may corrupt).
#pragma once

#include <array>
#include <cstddef>

namespace ea::sil {

class TempDir {
public:
    TempDir() noexcept;
    ~TempDir();
    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;

    const char* path() const noexcept { return path_.data(); }

    // Copies <src_dir>/<rel> to <path>/<rel>, creating one level of sub-directory if needed.
    bool copy_from(const char* src_dir, const char* rel) noexcept;
    // XORs one byte of <path>/<rel> with 0xFF.
    bool flip_byte(const char* rel, std::size_t offset) noexcept;

private:
    static constexpr std::size_t kMaxEntries = 32U;
    static constexpr std::size_t kMaxPath = 512U;
    bool remember(const char* p, bool is_dir) noexcept;

    std::array<char, kMaxPath> path_{};
    std::array<std::array<char, kMaxPath>, kMaxEntries> entries_{};
    std::array<bool, kMaxEntries> is_dir_{};
    std::size_t count_ = 0U;
};

}  // namespace ea::sil

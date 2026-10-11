#include "ea/sil/temp_dir.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ea/core/assert.hpp"

namespace ea::sil {

TempDir::TempDir() noexcept {
    const char* base = std::getenv("TMPDIR");
    const int n = std::snprintf(path_.data(), path_.size(), "%s/ea_sil_XXXXXX", (base != nullptr) ? base : "/tmp");
    EA_ASSERT(n > 0 && static_cast<std::size_t>(n) < path_.size());
    EA_ASSERT(mkdtemp(path_.data()) != nullptr);
}

TempDir::~TempDir() {
    for (std::size_t i = count_; i > 0U; --i) {
        if (is_dir_[i - 1U]) {
            (void)rmdir(entries_[i - 1U].data());
        } else {
            (void)unlink(entries_[i - 1U].data());
        }
    }
    (void)rmdir(path_.data());
}

bool TempDir::remember(const char* p, bool is_dir) noexcept {
    if (count_ >= kMaxEntries) {
        return false;
    }
    const std::size_t n = std::strlen(p);
    if (n >= kMaxPath) {
        return false;  // never store a truncated path (cleanup would miss it)
    }
    std::memcpy(entries_[count_].data(), p, n + 1U);
    is_dir_[count_] = is_dir;
    ++count_;
    return true;
}

bool TempDir::copy_from(const char* src_dir, const char* rel) noexcept {
    std::array<char, kMaxPath> src{};
    std::array<char, kMaxPath> dst{};
    if (std::snprintf(src.data(), src.size(), "%s/%s", src_dir, rel) >= static_cast<int>(src.size()) ||
        std::snprintf(dst.data(), dst.size(), "%s/%s", path_.data(), rel) >= static_cast<int>(dst.size())) {
        return false;
    }
    const char* slash = std::strrchr(rel, '/');
    if (slash != nullptr) {
        std::array<char, kMaxPath> dir{};
        const int len = static_cast<int>(slash - rel);
        if (std::snprintf(dir.data(), dir.size(), "%s/%.*s", path_.data(), len, rel) >= static_cast<int>(dir.size())) {
            return false;
        }
        struct stat st {};
        if (stat(dir.data(), &st) != 0) {
            if (mkdir(dir.data(), 0700) != 0 || !remember(dir.data(), true)) {
                return false;
            }
        }
    }
    std::FILE* in = std::fopen(src.data(), "rb");
    if (in == nullptr) {
        return false;
    }
    std::FILE* out = std::fopen(dst.data(), "wb");
    if (out == nullptr) {
        (void)std::fclose(in);
        return false;
    }
    bool ok = remember(dst.data(), false);
    std::array<unsigned char, 4096> buf{};
    std::size_t n = 0U;
    while (ok && (n = std::fread(buf.data(), 1U, buf.size(), in)) > 0U) {
        ok = std::fwrite(buf.data(), 1U, n, out) == n;
    }
    (void)std::fclose(in);
    ok = (std::fclose(out) == 0) && ok;
    return ok;
}

bool TempDir::flip_byte(const char* rel, std::size_t offset) noexcept {
    std::array<char, kMaxPath> p{};
    if (std::snprintf(p.data(), p.size(), "%s/%s", path_.data(), rel) >= static_cast<int>(p.size())) {
        return false;
    }
    std::FILE* f = std::fopen(p.data(), "r+b");
    if (f == nullptr) {
        return false;
    }
    bool ok = std::fseek(f, static_cast<long>(offset), SEEK_SET) == 0;
    const int c = ok ? std::fgetc(f) : EOF;
    ok = ok && c != EOF && std::fseek(f, static_cast<long>(offset), SEEK_SET) == 0 &&
         std::fputc(c ^ 0xFF, f) != EOF;
    return (std::fclose(f) == 0) && ok;
}

}  // namespace ea::sil

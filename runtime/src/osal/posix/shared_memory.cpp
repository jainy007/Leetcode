#include "ea/osal/shared_memory.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>

namespace ea::osal {

namespace {

bool name_valid(const char* name, std::size_t max) noexcept {
    if (name == nullptr || name[0] != '/') {
        return false;
    }
    const std::size_t len = std::strlen(name);
    return len > 1U && len < max && std::strchr(name + 1, '/') == nullptr;
}

}  // namespace

SharedMemory::SharedMemory(void* addr, std::size_t size, const char* unlink_name) noexcept
    : addr_(addr), size_(size) {
    if (unlink_name != nullptr) {
        std::strncpy(unlink_name_, unlink_name, kMaxName - 1U);
    }
}

Result<SharedMemory> SharedMemory::create(const char* name, std::size_t size) noexcept {
    if (!name_valid(name, kMaxName) || size == 0U) {
        return ErrorCode::kInvalidArgument;
    }
    const int fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        return ErrorCode::kOsError;
    }
    if (ftruncate(fd, static_cast<off_t>(size)) != 0) {
        (void)close(fd);
        (void)shm_unlink(name);
        return ErrorCode::kOsError;
    }
    void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    (void)close(fd);
    if (addr == MAP_FAILED) {  // NOLINT
        (void)shm_unlink(name);
        return ErrorCode::kOsError;
    }
    std::memset(addr, 0, size);
    return SharedMemory(addr, size, name);
}

Result<SharedMemory> SharedMemory::open(const char* name, std::size_t size) noexcept {
    if (!name_valid(name, kMaxName) || size == 0U) {
        return ErrorCode::kInvalidArgument;
    }
    const int fd = shm_open(name, O_RDWR, 0);
    if (fd < 0) {
        return ErrorCode::kOsError;
    }
    struct stat st {};
    if (fstat(fd, &st) != 0 || static_cast<std::size_t>(st.st_size) != size) {
        (void)close(fd);
        return ErrorCode::kLengthMismatch;
    }
    void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    (void)close(fd);
    if (addr == MAP_FAILED) {  // NOLINT
        return ErrorCode::kOsError;
    }
    return SharedMemory(addr, size, nullptr);
}

SharedMemory::SharedMemory(SharedMemory&& other) noexcept : addr_(other.addr_), size_(other.size_) {
    std::memcpy(unlink_name_, other.unlink_name_, kMaxName);
    other.addr_ = nullptr;
    other.size_ = 0U;
    other.unlink_name_[0] = '\0';
}

SharedMemory& SharedMemory::operator=(SharedMemory&& other) noexcept {
    if (this != &other) {
        release();
        addr_ = other.addr_;
        size_ = other.size_;
        std::memcpy(unlink_name_, other.unlink_name_, kMaxName);
        other.addr_ = nullptr;
        other.size_ = 0U;
        other.unlink_name_[0] = '\0';
    }
    return *this;
}

SharedMemory::~SharedMemory() {
    release();
}

void SharedMemory::release() noexcept {
    if (addr_ != nullptr) {
        (void)munmap(addr_, size_);
        addr_ = nullptr;
    }
    if (unlink_name_[0] != '\0') {
        (void)shm_unlink(unlink_name_);
        unlink_name_[0] = '\0';
    }
}

}  // namespace ea::osal

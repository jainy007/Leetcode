// POSIX shared memory region (shm_open + mmap), move-only RAII (rule MP-002).
#pragma once

#include <cstddef>

#include "ea/core/result.hpp"

namespace ea::osal {

class SharedMemory {
public:
    // Creates (O_EXCL) and sizes a region. The creator unlinks the name on destruction.
    static Result<SharedMemory> create(const char* name, std::size_t size) noexcept;
    // Opens an existing region; fails if its size differs from `size`.
    static Result<SharedMemory> open(const char* name, std::size_t size) noexcept;

    SharedMemory(SharedMemory&& other) noexcept;
    SharedMemory& operator=(SharedMemory&& other) noexcept;
    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;
    ~SharedMemory();

    void* data() const noexcept { return addr_; }
    std::size_t size() const noexcept { return size_; }

private:
    SharedMemory(void* addr, std::size_t size, const char* unlink_name) noexcept;
    void release() noexcept;

    static constexpr std::size_t kMaxName = 64U;
    void* addr_ = nullptr;
    std::size_t size_ = 0U;
    char unlink_name_[kMaxName] = {};
};

}  // namespace ea::osal

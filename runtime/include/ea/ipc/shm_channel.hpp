// Latest-value channel between processes over POSIX shared memory (rules MP-001, MP-004).
// Used for freedom from interference: a QM producer process publishes, an ASIL monitor process
// subscribes read-only in logic (the monitor never writes the producer's state).
#pragma once

#include <atomic>
#include <cstdint>
#include <new>

#include "ea/core/result.hpp"
#include "ea/ipc/seqlock.hpp"
#include "ea/osal/shared_memory.hpp"

namespace ea::ipc {

namespace detail {

inline constexpr std::uint32_t kChannelMagic = 0x45414348U;  // "EACH"

template <typename T>
struct ChannelLayout {
    std::atomic<std::uint32_t> magic;
    std::uint32_t payload_size;
    Seqlock<T> slot;
};

}  // namespace detail

template <typename T>
class ShmPublisher {
public:
    static Result<ShmPublisher> create(const char* name) noexcept {
        auto shm = osal::SharedMemory::create(name, sizeof(detail::ChannelLayout<T>));
        if (!shm.ok()) {
            return shm.error();
        }
        auto* layout = ::new (shm->data()) detail::ChannelLayout<T>{};
        layout->payload_size = static_cast<std::uint32_t>(sizeof(T));
        layout->magic.store(detail::kChannelMagic, std::memory_order_release);
        return ShmPublisher(std::move(shm).value(), layout);
    }

    void publish(const T& value) noexcept { layout_->slot.store(value); }

private:
    ShmPublisher(osal::SharedMemory shm, detail::ChannelLayout<T>* layout) noexcept
        : shm_(std::move(shm)), layout_(layout) {}

    osal::SharedMemory shm_;
    detail::ChannelLayout<T>* layout_;
};

template <typename T>
class ShmSubscriber {
public:
    static Result<ShmSubscriber> open(const char* name) noexcept {
        auto shm = osal::SharedMemory::open(name, sizeof(detail::ChannelLayout<T>));
        if (!shm.ok()) {
            return shm.error();
        }
        auto* layout = static_cast<detail::ChannelLayout<T>*>(shm->data());
        if (layout->magic.load(std::memory_order_acquire) != detail::kChannelMagic || layout->payload_size != sizeof(T)) {
            return ErrorCode::kProtocolError;
        }
        return ShmSubscriber(std::move(shm).value(), layout);
    }

    Result<T> latest() const noexcept { return layout_->slot.load(); }
    std::uint64_t version() const noexcept { return layout_->slot.version(); }

private:
    ShmSubscriber(osal::SharedMemory shm, detail::ChannelLayout<T>* layout) noexcept
        : shm_(std::move(shm)), layout_(layout) {}

    osal::SharedMemory shm_;
    detail::ChannelLayout<T>* layout_;
};

}  // namespace ea::ipc

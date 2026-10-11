#include "ea/plant/command_stub.hpp"

#include "ea/core/assert.hpp"
#include "ea/plant/signals.hpp"

namespace ea::plant {

CommandStub::CommandStub(std::uint32_t can_id, const e2e::P11Config& e2e) noexcept
    : id_(can_id), len_(e2e.length), prot_(e2e) {
    EA_ASSERT(len_ <= tx_.size());
    tx_ = values_;
}

void CommandStub::set(const can::Sig& sig, double value) noexcept { put(values_, sig, value); }

void CommandStub::step() noexcept {
    tx_ = values_;
    const Result<void> r = prot_.protect(ByteSpan(tx_.data(), len_));
    EA_ASSERT(r.ok());
}

}  // namespace ea::plant

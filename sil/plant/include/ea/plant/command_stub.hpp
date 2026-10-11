// Stimulus node for monitor SIL: sends a controller command message with values set by the test
// (encoded and E2E-protected with the plant's own tables), standing in for the controller.
#pragma once

#include <array>
#include <cstdint>

#include "ea/e2e/p11.hpp"
#include "ea/plant/acc_can.hpp"
#include "ea/plant/acc_observer.hpp"
#include "ea/plant/aeb_can.hpp"
#include "ea/plant/aeb_observer.hpp"
#include "ea/plant/plant.hpp"
#include "ea/plant/vehicle_can.hpp"

namespace ea::plant {

class CommandStub : public ControllerPort {
public:
    CommandStub(std::uint32_t can_id, const e2e::P11Config& e2e) noexcept;

    // Value of a signal in every following frame (physical units).
    void set(const can::Sig& sig, double value) noexcept;

    void on_frame(std::uint32_t /*can_id*/, ConstByteSpan /*payload*/) noexcept override {}
    void step() noexcept override;
    std::size_t tx_count() const noexcept override { return 1U; }
    std::uint32_t tx_can_id(std::size_t /*i*/) const noexcept override { return id_; }
    ConstByteSpan tx_frame(std::size_t /*i*/) const noexcept override { return ConstByteSpan(tx_.data(), len_); }

private:
    std::uint32_t id_;
    std::size_t len_;
    e2e::P11Protector prot_;
    std::array<std::uint8_t, 8> values_{};
    std::array<std::uint8_t, 8> tx_{};
};

class AccCommandStub final : public CommandStub {
public:
    AccCommandStub() noexcept : CommandStub(can::kAccCommandId, can::kAccCommandE2e) {}
};

class AebCommandStub final : public CommandStub {
public:
    AebCommandStub() noexcept : CommandStub(can::kAebCommandId, can::kAebCommandE2e) {}
};

}  // namespace ea::plant

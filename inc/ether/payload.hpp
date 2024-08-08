#pragma once
#include <cstdint>
#include <span>
#include <string>
namespace ether
{
    enum class ethertype : std::uint16_t
    {
        IPV4    = 0x8000,
        ARP     = 0x0806,
        IPV6    = 0x86DD,
        UNKNOWN = 0x0000
    };

    struct payload
    {
        [[nodiscard]] virtual std::span<uint8_t> get_data() noexcept = 0;
        [[nodiscard]] virtual std::span<const uint8_t> get_data() const noexcept = 0;
        [[nodiscard]] virtual constexpr ethertype get_type() const noexcept = 0;

        [[nodiscard]] virtual std::string dbg_string() const noexcept = 0;


        virtual ~payload() = default;
    };
}  // namespace ether

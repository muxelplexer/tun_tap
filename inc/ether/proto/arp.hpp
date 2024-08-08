#pragma once
#include "ether/payload.hpp"
#include "ether/proto/raw.hpp"
#include "mhl/sys/net.hpp"
#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace ether::proto::arp
{
    /*
     * see: https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
     */

    enum class operation : uint8_t
    {
        UNKNOWN = 0,
        REQUEST = 1,
        REPLY   = 2
    };

    /*
     * see RFC826
     */
    class payload final : public ether::proto::raw::payload
    {
    private:
        using mac_span = mhl::sys::net::mac_span;
        using mac_const_span = mhl::sys::net::mac_const_span;
        using ipv4_const_span = mhl::sys::net::ipv4_const_span;
        static constexpr size_t hwtype_field_len = 2;
        static constexpr size_t prot_field_len = 2;
        static constexpr size_t hwlen_field_len = 1;
        static constexpr size_t protlen_field_len = 1;
        static constexpr size_t op_field_len = 2;

    public:
        explicit payload(std::span<uint8_t> data)
            : ether::proto::raw::payload(data)
        {

        }
        ~payload() override = default;
        [[nodiscard]] inline constexpr ethertype get_type() const noexcept override
        {
            return ether::ethertype::ARP;
        }

        [[nodiscard]] inline mac_const_span get_src_mac_addr() const
        {
            constexpr auto src_pos = 8;
            return mac_const_span(this->get_internal_data().begin() + src_pos, hw_addr_len());
        }

        [[nodiscard]] inline ipv4_const_span get_src_ip_addr() const
        {
            constexpr auto dest_pos = 14;
            return ipv4_const_span(this->get_internal_data().begin() + dest_pos, prot_addr_len());
        }
        [[nodiscard]] inline mac_const_span get_dest_mac_addr() const
        {
            constexpr auto dest_pos = 22;
            return mac_const_span(this->get_internal_data().begin() + dest_pos, prot_addr_len());
        }
        [[nodiscard]] inline ipv4_const_span get_dest_ip_addr() const
        {
            constexpr auto dest_pos = 28;
            return ipv4_const_span(this->get_internal_data().begin() + dest_pos, prot_addr_len());
        }

        [[nodiscard]] inline arp::operation get_op() const
        {
            constexpr auto pos = hwtype_field_len + prot_field_len + hwlen_field_len + prot_field_len;
            return operation{this->get_addr<uint8_t>(pos)};
        }

        [[nodiscard]] inline std::uint16_t hw_addr_len() const
        {
            constexpr auto len_pos = hwtype_field_len + prot_field_len;
            return this->get_addr<uint16_t>(len_pos);
        }

        [[nodiscard]] inline std::uint16_t prot_addr_len() const
        {
            constexpr auto len_pos = hwtype_field_len + prot_field_len + hwlen_field_len;
            return this->get_addr<uint16_t>(len_pos);
        }

        [[nodiscard]] inline std::string dbg_string() const noexcept override
        {
            return std::format("[{}] SMAC {} -> DMAC {} // SIP {} -> DIP {}\n",
                    std::to_underlying(this->get_op()),
                    mhl::to_string(this->get_src_mac_addr()),
                    mhl::to_string(this->get_dest_mac_addr()),
                    mhl::to_string(this->get_src_ip_addr()),
                    mhl::to_string(this->get_dest_ip_addr())
            /*
            return std::format("[{}] Who has {}? Tell {} {}\n", 
                    std::to_underlying(this->get_op()),
                    mhl::to_string(this->get_src_mac_addr()),
                    mhl::to_string(this->get_src_ip_addr()),
                    mhl::to_string(this->get_dest_ip_addr())
            */
            );
        }
    };
}  // namespace ether::proto::arp

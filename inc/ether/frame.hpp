#pragma once
#include "ether/proto/arp.hpp"
#include "ether/proto/raw.hpp"
#include "mhl/sys/net.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <memory>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "ether/payload.hpp"

namespace ether
{
    constexpr size_t frame_size = 48;

    struct ether_data
    {
        ether::ethertype    ether_type;
        std::vector<std::uint8_t> data;
    };

    class frame
    {
    private:
        using mac_addr = mhl::sys::net::mac_address;
        using mac_span = mhl::sys::net::mac_span;
    public:
        explicit frame(const mac_span src, const mac_span dest, const ether::ethertype type, const uint32_t crc, std::span<std::uint8_t> data)
            : _ether_type{type}, _crc{crc}
        {
            std::copy_n(
                std::cbegin(src),
                mac_span::extent,
                std::begin(_mac_source)
            );
            std::copy_n(
                std::cbegin(dest),
                mac_span::extent,
                std::begin(_mac_dest)
            );

            switch(_ether_type)
            {
            case ethertype::ARP:
                _data = std::make_unique<ether::proto::arp::payload>(data);
                break;
            default:
                _data = std::make_unique<ether::proto::raw::payload>(data);
                break;
            }
        }
        frame() = default;

        [[nodiscard]] inline const ether::payload* get_data() const { return _data.get(); }
        [[nodiscard]] inline ether::payload* get_data() { return _data.get(); }

        [[nodiscard]] inline const mac_addr& get_src() const { return _mac_source; }
        [[nodiscard]] inline mac_addr& get_src() { return _mac_source; }

        [[nodiscard]] inline const mac_addr& get_dest() const { return _mac_dest; }
        [[nodiscard]] inline mac_addr& get_dest() { return _mac_dest; }

        [[nodiscard]] inline const ether::ethertype& get_type() const { return _ether_type; }
        [[nodiscard]] inline ether::ethertype& get_type() { return _ether_type; }

        [[nodiscard]] inline const uint32_t& get_crc() const { return _crc; }
        [[nodiscard]] inline uint32_t& get_crc() { return _crc; }

        [[nodiscard]] static frame parse(std::span<uint8_t> data);

    private:
        mac_addr                        _mac_source{};
        mac_addr                        _mac_dest{};
        ether::ethertype                _ether_type{};
        std::unique_ptr<ether::payload> _data{nullptr};
        uint32_t                        _crc{};
    };
}  // namespace ether

template<>
struct std::formatter<ether::ethertype>
{
    // parse the format string for this type:
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin(); // return position of } (hopefully there)
    }
    // format by always writing its value:
    auto format(const ether::ethertype& obj, std::format_context& ctx) const {

        auto format = [&ctx, &obj] (const std::string_view type) {
            return std::format_to(ctx.out(), "{}[{:#02x}]", type, std::to_underlying<ether::ethertype>(obj));
        };
        switch(obj)
        {
        case ether::ethertype::IPV4:
            return format("IPv4");
        case ether::ethertype::ARP:
            return format("ARP");
        case ether::ethertype::IPV6:
            return format("IPv6");
            break;
        default:
            break;
        }

        return format("UNKNOWN");
    }
};

template<>
struct std::formatter<mhl::sys::net::mac_address>
{
    // parse the format string for this type:
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin(); // return position of } (hopefully there)
    }
    // format by always writing its value:
    auto format(const mhl::sys::net::mac_address& obj, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
                obj[0], obj[1], obj[2], obj[3], obj[4], obj[5]
        );
    }
};

template<>
struct std::formatter<ether::frame>
{
    // parse the format string for this type:
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin(); // return position of } (hopefully there)
    }
    // format by always writing its value:
    auto format(const ether::frame& obj, std::format_context& ctx) const {
        std::format_to(ctx.out(), "{} | {} -> {}\t#{:08X}\n",
                obj.get_type(), obj.get_src(), obj.get_dest(), obj.get_crc()
        );
        auto data = obj.get_data();
        return std::format_to(ctx.out(), "{}", data->dbg_string());
    }
};

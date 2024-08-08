#include "ether/frame.hpp"
#include "ether/payload.hpp"
#include "mhl/sys/net.hpp"
#include <cstdint>
#include <print>
#include <span>

namespace ether
{
    frame frame::parse(std::span<uint8_t> data)
    {
        auto net_cast = [] <class T> (const uint8_t* datap)
        {
            using mhl::sys::net::ntoh;
            return ntoh<T>(*reinterpret_cast<const T*>(datap));
        };
        const mac_span dest{std::cbegin(data), std::cbegin(data) + 6};
        const mac_span src{std::cend(dest), std::cend(dest) + 6};
        const auto type_pos{std::cend(src)};
        const auto crc_pos{std::cend(data) - 4};
        const ethertype type{
            net_cast.template operator()<uint16_t>(type_pos.base())
        };
        const uint32_t crc{
            net_cast.template operator()<uint32_t>(crc_pos.base())
        };

        return frame{src, dest, type, crc, {(type_pos+2).base(), crc_pos.base()}};
    }
}  // namespace ether

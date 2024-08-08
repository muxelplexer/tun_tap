#pragma once
#include <algorithm>
#include <cstdint>
#include <format>
#include <span>
#include <sstream>
#include <string>
#include <vector>

#include "ether/payload.hpp"
#include "mhl/sys/net.hpp"

namespace ether::proto::raw
{
    class payload : public ether::payload
    {
    public:
        explicit payload(std::span<uint8_t> data)
            : ether::payload(), _data{std::cbegin(data), std::cend(data)}
        {

        }
        ~payload() override = default;

        [[nodiscard]] std::span<uint8_t> get_data() noexcept override
        {
            return _data;
        }

        [[nodiscard]] std::span<const uint8_t> get_data() const noexcept override
        {
            return _data;
        }

        [[nodiscard]] inline constexpr ethertype get_type() const noexcept override
        {
            return ether::ethertype::UNKNOWN;
        }

        [[nodiscard]] inline std::string dbg_string() const noexcept override
        {
            std::stringstream str{};
            std::for_each(
                    std::cbegin(_data),
                    std::cend(_data),
                    [&str](const unsigned char& ch)
                    {
                        str << std::format("{:02x}", ch);
                    }
            );
            str << '\n';
            return str.str();
        }
    protected:
        template<class T>
        auto net_cast(const uint8_t* datap) const
        {
            using mhl::sys::net::ntoh;
            return ntoh<T>(*reinterpret_cast<const T*>(datap));
        }
        template<class T>
        [[nodiscard]] T get_addr(const std::uint32_t offset) const noexcept;
        template<>
        [[nodiscard]] inline uint8_t get_addr(const std::uint32_t offset) const noexcept
        {
            return *reinterpret_cast<const uint8_t*>((std::cbegin(this->get_internal_data()) + offset).base());
        }

        template<>
        [[nodiscard]] inline uint16_t get_addr(const std::uint32_t offset) const noexcept
        {
            return net_cast<uint16_t>((std::cbegin(this->get_internal_data()) + offset).base());
        }

        template<>
        [[nodiscard]] inline uint32_t get_addr(const std::uint32_t offset) const noexcept
        {
            return net_cast<uint32_t>((std::cbegin(this->get_internal_data()) + offset).base());
        }
        std::vector<uint8_t>& get_internal_data() { return _data; }
        [[nodiscard]] const std::vector<uint8_t>& get_internal_data() const { return _data; }
    private:
        std::vector<uint8_t> _data;
    };
}  // namespace ether::proto::raw

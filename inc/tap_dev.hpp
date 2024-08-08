#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <fcntl.h>
#include <string_view>
#include <utility>

#include "mhl/sys/fd.hpp"

#include "ether/frame.hpp"

class tap_dev
{
private:
    [[nodiscard]] std::string init_device_name() const noexcept;
public:
    tap_dev()
        : tap_dev_name(init_device_name())
    {

    }

    [[nodiscard]] std::optional<std::string_view> get_name() const noexcept;
    [[nodiscard]] std::pair<std::uint8_t*, std::size_t> read() noexcept;

    inline mhl::sys::fd& get_fd()
    {
        return _fd;
    }

    constexpr static std::size_t MTU_SIZE = 1500;

private:
    mhl::sys::fd _fd{::open("/dev/net/tun", O_RDWR)};
    std::string tap_dev_name{};
    std::array<std::uint8_t, MTU_SIZE + ether::frame_size> _buf{};
};

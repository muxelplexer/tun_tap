#include "tap_dev.hpp"
#include <cstdint>
#include <cstring>
#include <exception>
#include <optional>
#include <print>
#include <string>
#include <linux/if_tun.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>

std::optional<std::string> tap_dev::get_name() const noexcept try
{
    struct ifreq ifr{};
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (const int err = ::ioctl(static_cast<int>(_fd), TUNSETIFF, static_cast<void*>(&ifr)); err < 0)
    {
        std::println("Error while trying to retrieve file-descriptor from '/dev/net/tun'.");
        return std::nullopt;
    }

    return ifr.ifr_name;
} catch (const std::exception& ex)
{
    return std::nullopt;
}

[[nodiscard]] std::pair<std::uint8_t*, std::size_t> tap_dev::read() noexcept
{
    auto nbytes = ::read(static_cast<int>(_fd), _buf.data(), _buf.max_size());
    return std::make_pair(_buf.data(), nbytes);
}

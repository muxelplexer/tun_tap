#include "tap_dev.hpp"
#include <cstdint>
#include <cstring>
#include <optional>
#include <print>
#include <string>
#include <linux/if_tun.h>
#include <net/if.h>
#include <string_view>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>

std::string tap_dev::init_device_name() const noexcept
{
    struct ifreq ifr{};
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (const int err = ::ioctl(static_cast<int>(_fd), TUNSETIFF, static_cast<void*>(&ifr)); err < 0)
    {
        std::println("Error while trying to retrieve file-descriptor from '/dev/net/tun'.");
        return "";
    }
    return ifr.ifr_name;
}
std::optional<std::string_view> tap_dev::get_name() const noexcept
{
    return tap_dev_name == "" ? std::nullopt : std::make_optional<std::string_view>(tap_dev_name);
}

[[nodiscard]] std::pair<std::uint8_t*, std::size_t> tap_dev::read() noexcept
{
    auto nbytes = ::read(static_cast<int>(_fd), _buf.data(), _buf.max_size());
    return std::make_pair(_buf.data(), nbytes);
}

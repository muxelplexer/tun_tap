#include "mhl/sys/netlink.hpp"
#include "dotenv/dotenv.hpp"
#include "tap_dev.hpp"
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <optional>
#include <print>
#include <stdexcept>

#include <fcntl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int main() try
{
    dotenv::load("../");
    auto dot_env_vars = dotenv::get_variables();
    for (const auto env_var : dot_env_vars)
    {
        // dotenv::getenv returns a std::optional object.
        // Since we know that these variables got set it's safe to call .value() on it
        std::println("{}={}", env_var, dotenv::get_env(std::string(env_var)).value());
    }

    tap_dev tun_dev{};
    auto tun_dev_name_opt = tun_dev.get_name();
    if (tun_dev_name_opt == std::nullopt)
    {
        std::println("Unable to retrieve TAP device's name. Aborting'");
        return EXIT_FAILURE;
    } else
    {
        std::println("Created TAP device {} with fd {}.", *tun_dev_name_opt, tun_dev.get_fd().get());
    }

    mhl::sys::net::netlink_sock nl_sock(NETLINK_ROUTE);
    auto nl_addr{mhl::sys::net::netlink_sock::create_sockaddr(RTMGRP_LINK | RTMGRP_IPV4_IFADDR)};

    if (auto err = nl_sock.bind_sock(reinterpret_cast<sockaddr*>(&nl_addr)); err != std::nullopt)
    {
        std::println("FATAL ERROR: {}", *err);
        return EXIT_FAILURE;
    }

    if (auto err = nl_sock.set_dev_ip(*tun_dev_name_opt, "10.10.10.1"); err != std::nullopt)
    {
        std::println("FATAL ERROR: {}", *err);
        return EXIT_FAILURE;
    }

    if (auto err = nl_sock.set_link(*tun_dev_name_opt, true); err != std::nullopt)
    {
        std::println("FATAL ERROR: {}", *err);
        return EXIT_FAILURE;
    }

    int i = 0;
    while(true)
    {
        auto [data, data_len] = tun_dev.read();
        if (data_len > 0)
        {
            std::println("Got data!");
            for (size_t j = 0; j < data_len; ++j)
            {
                std::print("{:02x} ", *(data + j));
            }
        }

        std::println("{}", i);
    }

    return EXIT_SUCCESS;
} catch (const std::exception& ex)
{
    std::println("FATAL ERROR: {}", ex.what());
    return EXIT_FAILURE;
}
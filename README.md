# TUN/TAP Experimentation

This repository serves as a means to test & play around with TUN/TAP interfaces,
implementing my own TCP/IP stack.

If you do not want to run the binary as root you will need to add the `NET_ADMIN`
capability to it:

```sh
sudo setcap cap_net_admin+ep build/tun_tap
```

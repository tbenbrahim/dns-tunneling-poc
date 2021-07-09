# DNS Tunneling POC

This is a proof of concept for a DNS tunnelling client and server.

## Running

### Running on Linux

On systems using `systemd`, you will have to temporarily turn off the systemd resolver to free up port 53 (used by the POC server).

```shell
sudo systemctl stop systemd-resolved
```

Since the POC server really cannot resolve anything, you may want to edit `/etc/resolv.conf` to point to a real server, such as `8.8.8.8`

### Running on WSL

WSL requires Internet Connection Sharing which uses port 53. It is therefore not possible to run the server.

## Open Source Software  used

Base32 encoder/decoder from [Google Authenticator PAM module](https://github.com/google/google-authenticator-libpam)

# Exercise 3.2 Terminal Outputs

1. `strace -e` with option `trace=network`, `curl -s` for silencing curl's progress bar

```bash
strace -e trace=network curl -s http://example.com > /dev/null
```

2. Notes
    - first `socket` and `connect` commands tried to connect to `nscd` - Name Service Cache Daemon, the local DNS caching service, and failed because it wasn't running
    - fall back to standard DNS resolution via UDP (`SOCK_DGRAM`), and `curl` uses some `socketpair`s for running asynchronous DNS resolvers internally, to speed things up
    - once DNS resolution was complete, the socket for the connection to `example.com` was opened and we got a fd of 5.
    - Some `setsockopt` options for the socket
    - `connect(5, ...)` connects to the server (TCP handshake) with non-blocking mechanism [-1 EINPROGRESS returned]
    - `getsockopt` asks if the TCP handshake failed, gets 0 -> no errors, confirms connection established
    - `getpeername` and `getsockname` -> peername returns the peer/remote's details, sockname returns the local side's socket and port details
    - `sendto(5, ...)` sends the request headers to the fd 5 (socket) -> length 76
        - `GET / HTTP/1.1\r\nHOST: example.com ...`
    - `recvfrom(5, ...)` receives the response from the same socket -> length 273
        - `HTTP/1.1 200 OK\r\nDate: Wed, 16 Sep 2026, ...`
    - When syscalls give the kernel file descriptors and pointers to memory locations for writing outputs (out parameters), `strace` resolves the pointers to memory and prints out what's in the memory locations in a human-readable format
    - This is the reason we can see the output of `recvfrom()` inside the call, it contains the pointer, but `strace` went the extra mile and writes out what's in the memory location for us in its output.
    - Inside the arguments of `read`, `recvfrom` and `get*` calls,
        - The process provided an empty bucket and it's capacity
        - The kernel filled that bucket
        - `strace` peeks inside the bucket afterwards, renders the contents inside the parentheses
    - The `[128 => 16]` says that the kernel got 128 bytes of memory space to fill in, but it only used 16 bytes, because the struct `{sa_family=AF_INET, sin_port=htons(43096), sin_addr=inet_addr("172.22.194.156")}` only needed that much.

3. Full Output

```text
socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
connect(3, {sa_family=AF_UNIX, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
connect(3, {sa_family=AF_UNIX, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP) = 3
socketpair(AF_UNIX, SOCK_STREAM, 0, [3, 4]) = 0
socketpair(AF_UNIX, SOCK_STREAM, 0, [5, 6]) = 0
socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) = 5
setsockopt(5, SOL_TCP, TCP_NODELAY, [1], 4) = 0
setsockopt(5, SOL_SOCKET, SO_KEEPALIVE, [1], 4) = 0
setsockopt(5, SOL_TCP, TCP_KEEPIDLE, [60], 4) = 0
setsockopt(5, SOL_TCP, TCP_KEEPINTVL, [60], 4) = 0
connect(5, {sa_family=AF_INET, sin_port=htons(80), sin_addr=inet_addr("172.66.147.243")}, 16) = -1 EINPROGRESS (Operation now in progress)
getsockopt(5, SOL_SOCKET, SO_ERROR, [0], [4]) = 0
getpeername(5, {sa_family=AF_INET, sin_port=htons(80), sin_addr=inet_addr("172.66.147.243")}, [128 => 16]) = 0
getsockname(5, {sa_family=AF_INET, sin_port=htons(43096), sin_addr=inet_addr("172.22.194.156")}, [128 => 16]) = 0
sendto(5, "GET / HTTP/1.1\r\nHost: example.co"..., 75, MSG_NOSIGNAL, NULL, 0) = 75
recvfrom(5, "HTTP/1.1 200 OK\r\nDate: Wed, 16 S"..., 102400, 0, NULL, NULL) = 872
+++ exited with 0 +++
```

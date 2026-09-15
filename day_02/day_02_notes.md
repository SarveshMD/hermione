# Notes - Day 2

## Gemini Explanation Snapshots

### The Flow

```text
[ Domain Name: cloudflare.com ]
             │
             ▼  1. DNS Resolution (Iterative Walk: Root → TLD → Authoritative)
[ IP Address: 104.16.132.229 ]
             │
             ▼  2. Transport Layer (TCP 3-Way Handshake via Berkeley Sockets)
[ Kernel Socket Established ]
             │
             ▼  3. Encryption & Negotiation (TLS 1.3 1-RTT Handshake + ALPN)
[ Encrypted Session Ready for HTTP/2 ]
```

### Berkeley Socket Lifecycle

#### The Server Lifecycle (bind, listen, accept)

- socket(): Allocates an inode and kernel memory for socket data structures (receive/send ring buffers).

- bind(): Associates the socket with a specific network interface IP and local port number.

- listen(): Moves the socket from an active state (initiator) to a passive state (listener). It creates two distinct queues inside the kernel:
    1. SYN Queue (Incomplete Connection Queue): Holds TCP handshakes in flight (client sent SYN, server answered SYN-ACK, waiting for final ACK). The socket is temporarily in SYN_RCVD.

    2. Accept Queue (Completed Connection Queue): Holds fully established connections (ESTABLISHED state) whose 3-way handshake finished, waiting for user space to claim them.

- accept(): Does not perform the TCP handshake. It merely dequeues an already-established connection from the Accept Queue and creates a brand-new file descriptor dedicated to talking to that specific client, leaving the original listener socket free to keep listening.

#### Recv-Q and Send-Q

In standard connected sockets, Recv-Q and Send-Q measure byte buffers:

- Connected Socket (ESTAB):
    - Recv-Q: Bytes received by the kernel network stack, copied into the socket receive buffer, but not yet read by your application via read()/recv().

    - Send-Q: Bytes written by the application via write()/send(), buffered in kernel space, but not yet acknowledged by the peer.

- Listening Socket (LISTEN): The kernel completely changes the meaning of these columns!
    - Send-Q: The configured maximum backlog limit (the size cap of the Accept Queue).

    - Recv-Q: The current number of established connections sitting in the Accept Queue waiting for the application to call accept().

## Experiments

### Lab 2.1: Berkeley Socket Mapping with `ss` - [terminal_1.md](terminal_1.md)

### Lab 2.2: Hierarchical DNS Walk - [terminal_2.md](terminal_2.md)

### Lab 2.3: TLS 1.3 Handshake Sniffing - [terminal_3.md](terminal_3.md)

# Exercise 2.1 Terminal Outputs

1. Run the nc listener [tiny server]

```bash
nc -l -p 8080 < /dev/null &
```

2. Socket Statistics [Tcp, Udp, Listening sockets, show Pids and Numeric ports]

```bash
ss -tulpn
```

```text
Netid      State       Recv-Q       Send-Q              Local Address:Port              Peer Address:Port      Process
udp        UNCONN      0            0                   127.0.0.53%lo:53                     0.0.0.0:*
udp        UNCONN      0            0                  10.255.255.254:53                     0.0.0.0:*
udp        UNCONN      0            0                       127.0.0.1:323                    0.0.0.0:*
udp        UNCONN      0            0                           [::1]:323                       [::]:*
tcp        LISTEN      0            1                         0.0.0.0:8080                   0.0.0.0:*          users:(("nc",pid=77608,fd=3))
tcp        LISTEN      0            511                     127.0.0.1:42209                  0.0.0.0:*          users:(("MainThread",pid=67327,fd=29))
tcp        LISTEN      0            511                     127.0.0.1:41223                  0.0.0.0:*          users:(("MainThread",pid=66826,fd=23))
tcp        LISTEN      0            1000               10.255.255.254:53                     0.0.0.0:*
tcp        LISTEN      0            4096                127.0.0.53%lo:53                     0.0.0.0:*
tcp        LISTEN      0            128                       0.0.0.0:22                     0.0.0.0:*
tcp        LISTEN      0            128                          [::]:22                        [::]:*
```

3. Connect to localhost:8080 [tiny client]

```bash
nc 127.0.0.1 8080
```

4. Socket Statistics [tcp, all types of sockets, numeric]

```bash
ss -tan | grep "8080"
```

```text
LISTEN 0      1             0.0.0.0:8080       0.0.0.0:*
ESTAB  0      0           127.0.0.1:52620    127.0.0.1:8080
ESTAB  0      0           127.0.0.1:8080     127.0.0.1:52620
```

- Shows two ESTAB connections:
    - local ephemeral port to peer 8080 port is client
    - local 8080 port to peer ephemeral port is server
- nc is still listening on the same port

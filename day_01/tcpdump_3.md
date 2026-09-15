# Exercise 3: nc into an inactive port to capture RST

```bash
sudo tcpdump -nnvv -i lo port 9999
```

```text
tcpdump: listening on lo, link-type EN10MB (Ethernet), snapshot length 262144 bytes
21:58:17.342134 IP (tos 0x0, ttl 64, id 58638, offset 0, flags [DF], proto TCP (6), length 60)                                                                  127.0.0.1.34420 > 127.0.0.1.9999: Flags [S], cksum 0xfe30 (incorrect -> 0x2860), seq 1676447712, win 65495, options [mss 65495,sackOK,TS val 857625395 ecr 0,nop,wscale 7], length 0
    127.0.0.1.34420 > 127.0.0.1.9999: Flags [S], cksum 0xfe30 (incorrect -> 0x2860), seq 1676447712, win 65495, options [mss 65495,sackOK,TS val 857625395 ecr 0,nop,wscale 7], length 0

21:58:17.342150 IP (tos 0x0, ttl 64, id 0, offset 0, flags [DF], proto TCP (6), length 40)
    127.0.0.1.9999 > 127.0.0.1.34420: Flags [R.], cksum 0x107d (correct), seq 0, ack 1676447713, win 0, length 0
^C
2 packets captured
4 packets received by filter
0 packets dropped by kernel
```

## Netcat (nc)

```bash
nc -vz localhost 9999
```

```text
nc: connect to localhost (127.0.0.1) port 9999 (tcp) failed: Connection refused
```

> Capturing the same into a .pcap file and examining from Wireshark [day1_reset.pcap]

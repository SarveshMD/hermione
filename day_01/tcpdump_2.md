# Exercise 2:

```bash
sudo !!
sudo tcpdump -nnvv -i any port 80
curl http://example.com
```

```text
tcpdump: data link type LINUX_SLL2
tcpdump: listening on any, link-type LINUX_SLL2 (Linux cooked v2), snapshot length 262144 bytes

18:38:19.976647 lo    In  IP (tos 0x0, ttl 64, id 16087, offset 0, flags [DF], proto TCP (6), length 60)
    127.0.0.1.48410 > 127.0.0.1.80: Flags [S], cksum 0xfe30 (incorrect -> 0x92b0), seq 1941366812, win 65495, options [mss 65495,sackOK,TS val 849441138 ecr 0,nop,wscale 7], length 0

18:38:19.976659 lo    In  IP (tos 0x0, ttl 64, id 0, offset 0, flags [DF], proto TCP (6), length 60)
    127.0.0.1.80 > 127.0.0.1.48410: Flags [S.], cksum 0xfe30 (incorrect -> 0x3f31), seq 2186751247, ack 1941366813, win 65483, options [mss 65495,sackOK,TSval 849441138 ecr 849441138,nop,wscale 7], length 0

18:38:19.976667 lo    In  IP (tos 0x0, ttl 64, id 16088, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.48410 > 127.0.0.1.80: Flags [.], cksum 0xfe28 (incorrect -> 0x65ed), seq 1, ack 1, win 512, options [nop,nop,TS val 849441138 ecr 849441138],length 0

18:38:19.976750 lo    In  IP (tos 0x0, ttl 64, id 16089, offset 0, flags [DF], proto TCP (6), length 125)
    127.0.0.1.48410 > 127.0.0.1.80: Flags [P.], cksum 0xfe71 (incorrect -> 0x0f09), seq 1:74, ack 1, win 512, options [nop,nop,TS val 849441139 ecr 849441138], length 73: HTTP, length: 73
        GET / HTTP/1.1
        Host: localhost
        User-Agent: curl/7.81.0
        Accept: */*

18:38:19.976756 lo    In  IP (tos 0x0, ttl 64, id 11077, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.80 > 127.0.0.1.48410: Flags [.], cksum 0xfe28 (incorrect -> 0x65a2), seq 1, ack 74, win 512, options [nop,nop,TS val 849441139 ecr 849441139], length 0

18:38:19.979969 lo    In  IP (tos 0x0, ttl 64, id 11078, offset 0, flags [DF], proto TCP (6), length 209)
    127.0.0.1.80 > 127.0.0.1.48410: Flags [P.], cksum 0xfec5 (incorrect -> 0xa1ee), seq 1:158, ack 74, win 512, options [nop,nop,TS val 849441142 ecr 849441139], length 157: HTTP, length: 157
        HTTP/1.0 200 OK
        Server: SimpleHTTP/0.6 Python/3.10.12
        Date: Mon, 14 Sep 2026 13:08:19 GMT
        Content-type: text/html; charset=utf-8
        Content-Length: 2462

18:38:19.980003 lo    In  IP (tos 0x0, ttl 64, id 16090, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.48410 > 127.0.0.1.80: Flags [.], cksum 0xfe28 (incorrect -> 0x6500), seq 74, ack 158, win 511, options [nop,nop,TS val 849441142 ecr 849441142], length 0

18:38:19.980034 lo    In  IP (tos 0x0, ttl 64, id 11079, offset 0, flags [DF], proto TCP (6), length 2514)
    127.0.0.1.80 > 127.0.0.1.48410: Flags [P.], cksum 0x07c7 (incorrect -> 0x371b), seq 158:2620, ack 74, win 512, options [nop,nop,TS val 849441142 ecr 849441142], length 2462: HTTP

18:38:19.980037 lo    In  IP (tos 0x0, ttl 64, id 16091, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.48410 > 127.0.0.1.80: Flags [.], cksum 0xfe28 (incorrect -> 0x5a88), seq 74, ack 2620, win 729, options [nop,nop,TS val 849441142 ecr 849441142], length 0

18:38:19.980073 lo    In  IP (tos 0x0, ttl 64, id 11080, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.80 > 127.0.0.1.48410: Flags [F.], cksum 0xfe28 (incorrect -> 0x5b60), seq 2620, ack 74, win 512, options [nop,nop,TS val 849441142 ecr 849441142], length 0

18:38:19.980365 lo    In  IP (tos 0x0, ttl 64, id 16092, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.48410 > 127.0.0.1.80: Flags [F.], cksum 0xfe28 (incorrect -> 0x5a86), seq 74, ack 2621, win 729, options [nop,nop,TS val 849441142 ecr 849441142], length 0

18:38:19.980381 lo    In  IP (tos 0x0, ttl 64, id 11081, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.80 > 127.0.0.1.48410: Flags [.], cksum 0xfe28 (incorrect -> 0x5b5f), seq 2621, ack 75, win 512, options [nop,nop,TS val 849441142 ecr 849441142], length 0

^C

12 packets captured
24 packets received by filter
0 packets dropped by kernel
```

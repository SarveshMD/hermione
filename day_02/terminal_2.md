# Exercise 2.2 Terminal Outputs

## 1. DNS Resolution (Iterative)

```bash
dig +trace www.apple.com
```

```text
; <<>> DiG 9.18.39-0ubuntu0.22.04.6-Ubuntu <<>> +trace www.apple.com
;; global options: +cmd
.                       87151   IN      NS      l.root-servers.net.
.                       87151   IN      NS      f.root-servers.net.
.                       87151   IN      NS      d.root-servers.net.
.                       87151   IN      NS      c.root-servers.net.
.                       87151   IN      NS      e.root-servers.net.
.                       87151   IN      NS      b.root-servers.net.
.                       87151   IN      NS      m.root-servers.net.
.                       87151   IN      NS      g.root-servers.net.
.                       87151   IN      NS      i.root-servers.net.
.                       87151   IN      NS      j.root-servers.net.
.                       87151   IN      NS      k.root-servers.net.
.                       87151   IN      NS      a.root-servers.net.
.                       87151   IN      NS      h.root-servers.net.
.                       87151   IN      RRSIG   NS 8 0 518400 20260928050000 20260915040000 57780 . eVt9DX7RJEjbzgJ3QfGjRTADZYK//92v/tyLMuAB4dH/8Rxl9Ya5XQja 6yamdJnaUu9zYBg/FK0wUDaem1KWXfZOxkHTSpSLkAAThHhCegrtEWkd XwY7Tj0laGQsFATEXDt+LJkpEGEiqmwlCEhzWeaJvyWt50fuu342+YeO eU7tAojeuo9AXVY7T6PmicqTjDfAa0uIIgbb2J12TfBHW40CVyfKRvNa Sw6ZfKmprpidbqRaar5xNHfD65IKtsVJqhWGKv4fSnigYITR6dQ/MOfB f/nUmMUnkw7fuAqIdYnV9omVGna7d4os/tHuSzbyGoDQF/83CPa04vwJ GU8w0A==
;; Received 1097 bytes from 10.255.255.254#53(10.255.255.254) in 7 ms

www.apple.com.          137     IN      CNAME   www-apple-com.v.aaplimg.com.
www-apple-com.v.aaplimg.com. 188 IN     CNAME   www.apple.com.edgekey.net.
www.apple.com.edgekey.net. 19   IN      CNAME   e6858.dsce9.akamaiedge.net.
e6858.dsce9.akamaiedge.net. 5   IN      A       23.200.48.248
.                       87151   IN      NS      m.root-servers.net.
.                       87151   IN      NS      f.root-servers.net.
.                       87151   IN      NS      j.root-servers.net.
.                       87151   IN      NS      k.root-servers.net.
.                       87151   IN      NS      e.root-servers.net.
.                       87151   IN      NS      c.root-servers.net.
.                       87151   IN      NS      i.root-servers.net.
.                       87151   IN      NS      g.root-servers.net.
.                       87151   IN      NS      l.root-servers.net.
.                       87151   IN      NS      a.root-servers.net.
.                       87151   IN      NS      b.root-servers.net.
.                       87151   IN      NS      d.root-servers.net.
.                       87151   IN      NS      h.root-servers.net.
.                       87151   IN      RRSIG   NS 8 0 518400 20260928050000 20260915040000 57780 . eVt9DX7RJEjbzgJ3QfGjRTADZYK//92v/tyLMuAB4dH/8Rxl9Ya5XQja 6yamdJnaUu9zYBg/FK0wUDaem1KWXfZOxkHTSpSLkAAThHhCegrtEWkd XwY7Tj0laGQsFATEXDt+LJkpEGEiqmwlCEhzWeaJvyWt50fuu342+YeO eU7tAojeuo9AXVY7T6PmicqTjDfAa0uIIgbb2J12TfBHW40CVyfKRvNa Sw6ZfKmprpidbqRaar5xNHfD65IKtsVJqhWGKv4fSnigYITR6dQ/MOfB f/nUmMUnkw7fuAqIdYnV9omVGna7d4os/tHuSzbyGoDQF/83CPa04vwJ GU8w0A==
;; Received 1210 bytes from 198.41.0.4#53(a.root-servers.net) in 0 ms
```

## 2. Pinging www.apple.com

```bash
ping www.apple.com
```

```text
PING e6858.dsce9.akamaiedge.net (23.200.48.248) 56(84) bytes of data.
64 bytes from a23-200-48-248.deploy.static.akamaitechnologies.com (23.200.48.248): icmp_seq=1 ttl=49 time=52.8 ms
64 bytes from a23-200-48-248.deploy.static.akamaitechnologies.com (23.200.48.248): icmp_seq=2 ttl=49 time=50.8 ms
^C
--- e6858.dsce9.akamaiedge.net ping statistics ---
2 packets transmitted, 2 received, 0% packet loss, time 1031ms
rtt min/avg/max/mdev = 50.849/51.821/52.794/0.972 ms
```

- It's rewarding to see that when I `ping www.apple.com`, we hit the same IPv4 Address as what we got from the `dig` command
- `ping` probably just looked `www.apple.com` up from the local resolver, and got back what we just populated it with using the `dig` command

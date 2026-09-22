# Notes - Day 7

## Experiments

- Lab 7.1: High Load, Low CPU - [terminal_1.md](terminal_1.md)

- Lab 7.2: Diagnostic Tools - [terminal_2.md](terminal_2.md)

## Final Checklist Answers

### 1. What exact network packets leave my machine when I run `curl https://azure.microsoft.com`?

   1. The URL needs to be resolved into an IP. Kernel checks local DNS cache, and then runs a Recursive Name Resolution and gets the IP (A UDP packet).
   2. A TCP `SYN` packet is sent to the server. Upon receiving the `SYN-ACK` from the server, the `ACK` is sent and a 3-way TCP handshake is completed.
   3. A TLS Handshake is initiated (`ClientHello`), cryptographic keys are exchanged, and a secure connection is established.
   4. `curl` then issues an encrypted `GET /` HTTP request to the server, and the response is streamed out `stdout`.

### 2. What is the difference between a process context switch and a thread context switch?

   > The Translation Lookaside Buffer (TLB) is a type of cache inside the CPU that stores recent mappings or translations between virtual and physical memory.

   1. Threads share the same virtual address space. The CPU's Translation Lookaside Buffer (TLB) does not need to be flushed. So there's less overhead on a thread context switch.
   2. Processes have completely different virtual address spaces. A process context switch invalidates the entire TLB, resulting in high TLB misses, increasing latency, and walks the RAM page tables for every instruction and data access until the cache catches up again.

### 3. Why does `kill -9` succeed when an application is locked in an infinite loop, but fails when stuck in state `D`?

   1. The kernel refuses to kill a process in state `D` (Uninterruptible Sleep)
   2. It is because a process enters state D when it awaits an I/O operation - like a disk read/write, communication over network interfaces, etc,.
   3. Killing a process when it's in an active connection with an I/O device could corrupt the device, so processes aren't killed in `D` state.

### 4. How does an epoll-based web server handle 100,000 concurrent open sockets without crashing the CPU?
   1. An epoll-based web server handles 100,000 concurrent open sockets without crashing the CPU because it replaces the old O(N) socket scanning with an event-driven model.
   2. Sockets are registered once inside a kernel-space **Red-Black Tree** via `epoll_ctl()`.
   3. When packets arrive, hardware interrupts trigger kernel callbacks that place only the sockets that have data to be read by the user-space onto a **Ready List**..
   4. Calling `epoll_wait()` returns in O(k) time, handing the application only the sockets that actually have data to read.
   5. This leaves the CPU idle instead of constantly scanning all 100,000 sockets.

### 5. If two Docker containers on the same host cannot communicate, what 3 CLI commands do you run first to pinpoint the exact failure point?

   > These were my first thoughts because I'd worked on namespaces I created locally, but for Docker containers, it should be `docker exec <container_name> ...`

   1. `sudo ip netns exec <container_namespace> ping -c 3 <other-container-ip>`
      - Try to `ping` the peer container if you know the IP. To find the IP out, use step 2.
   2. `sudo ip netns exec <container_namespace> ip addr`
      - List the network interface statuses and links
      - See if the `eth0` (if connected over the internet) or the veth pair that connecs the container to the virtual switch (like `docker0`) are `UP` or `DOWN`
      - See if they have a configured IP address to talk to.
   3. Configure the IP or `veth` pairs to connect them the way they should be. There's no one way to connect two containers, but usually they're connected with a virtual switch like `docker0` in the middle, with a veth pair connecting each container on one end and the virtual switch on the other.
   4. This way, if there are more containers that need to be in the same virtual network, it's as simple as creating a new veth pair for that container.

   #### The Docker Way

   5. `docker exec <container_name> ip addr`
      - Look if all container interfaces are `UP`
      - Look if their IPs are on the same subnet
      - Ensure they're all connected to the expected Docker bridge network
   6. `ip link show docker0`
      - Verify the bridge is active and both `veth` endpoints are plugged in
   7. `sudo iptables -L FORWARD -n -v`
      - Gets the forwarding rules, without reverse DNS lookups, and verbose
      - `ping` or `curl` from one container to another and observe the packet counter `pkts` column on `iptables` command. If we find a rule for `DROP` or `REJECT` increase, we'll have found the problem => The linux firewall is throwing out our packets.


## Incident Report - Cloudflare ReDOS Outage (July 2, 2019)

I'm writing out whatever I gathered from reading about this.

### 1. Trigger

- An update to the Web Application Firewall's rule targeting Cross-Site Scripting (XSS) contained a poorly constrained regular expression.
- The regular expression engine experienced a catastrophic backtracking (*O(2<sup>n</sup>)* time complexity) when evaluating requests

### 2. Failure propagation

- NGINX workers were trapped in user-space executing the regex computation
- Incoming requests didn't get `accept()` or `read()` calls.
- Socket backlogs overflowed, and the kernel dropped incoming TCP `SYN` packets and closed connections with `RST`.

### 3. Response to Clients

- Requests to the server timed out and started and the reverse proxies started sending out 502/504 Bad Gateway/Gateway Timeout errors.
> Note: Reverse Proxy Servers sit in front of our servers, facing the public internet to protect, hide and optimize the server

### 4. Metrics

- `top` / `vmstat`: `%us` spiked to 100% across all cores; `r` (runnable queue) exploded; `b` (blocked) remained 0; `%wa` remained 0%.
- `ss -lnt`: `Send-Q` on ports 80/443 hit maximum capacity (backlog limit reached).
- `strace -p <pid>`: 0 syscalls emitted (process trapped in user space).

### 5. Net Impact

- Global HTTP 502 Bad Gateway errors across ~50% of Cloudflare traffic, peaking at ~82%, for approximately 27 minutes.
- Approximately 10% of all internet traffic briefly froze.
- Impacted over 1 billion users.

## First Response Steps
> When a Linux VM or container starts throwing HTTP 502/504 errors.
> To be executed ON THE SERVER

### 1. Isolate Local App

   - Run `ss -tlpn` to find the listening port and server application's PID
   - `curl -Iv http://localhost:<app_port>/`
   - Look for:
     - Connection Refused: The process died or crashed, or is listening on a different interface.
     - Returns any HTTP Status (even 500 or 404): The app is alive and parsing HTTP. 502/504 is on the load balancer timeout, proxy configuration or the network path.

### 2. Check Scheduler State and Saturation

   - Run `vmstat 1 3`
   - If `r` (runnable) is significantly higher than CPU Core count => CPU starvation. CPU is overloaded, causing requests to queue up and time out
   - If `b` (blocked) > 0, processes are blocked waiting on hardware I/O
   - High `us`: Bad application logic, heavy compute, ReDoS regex
   - High `wa`: Disk subsystem bottleneck. CPU is idle because storage cannot keep up

### 3. Check Memory Pressure and Swap

   - Check `available` in `free -m`. If it's near zero, the kernel is running out of physical memory.
   - `si` / `so` in `vmstat`
     - Both zero => clean
     - Either > 0 continuously => Actively reading and writing RAM pages into disk. Latency is thousands of times slower than RAM, causing app execution times to skyrocket => Timeouts => HTTP 502/504 errors

### 4. Check Sockets and Listen Backlogs

   - Check `ss -lnt` & `ss -s`
   - In `ss -lnt`
     - Look at the Recv-Q and Send-Q columns of listening app port
     - Send-Q represents the maximum backlog limit (LISTEN)
     - Recv-Q represents the number of connections waiting in line to be `accept()`-ed by the application.
     - If Recv-Q >= Send-Q => Listening queue is full. Application cannot process connections fast enough, and new incoming packets are being silently dropped.
   - In `ss -s`
     - Look for high TIME-WAIT and CLOSE-WAIT

### 5. Check Kernel logs for OOM Killer

   - Run `dmesg -T | tail -30`
     - Look for Out of Memory: Kill Process <PID> ...
     - If the OOM killer assassinated our worker or background processes, that might be the problem.


## Gemini Explanations

### 1. The Core Mental Model: Brendan Gregg’s USE Method

When production breaks, you have dozens of subsystems (CPU, memory, storage disks, network interfaces, controllers, buses). Trying to guess which one is failing leads to confirmation bias.

The **USE Method** standardizes how you interrogate every hardware and OS component:

- **Utilization**: The percentage of time a resource was busy, or the percentage of its capacity currently in use (e.g., CPU busy time, disk `%util`, memory allocated).

- **Saturation**: The degree to which extra work is waiting in a queue because the resource is maxed out (e.g., runnable threads waiting for a core, disk queue depth, backlog on a socket listen queue).

- **Errors**: Direct failures logged or counted (e.g., dropped packets on an interface, disk I/O read errors, TCP retransmissions).

If a system is slow or timing out, one or more resources are almost always either saturated or throwing errors.

### 2. Linux Load Average: The "High Load, Low CPU" Paradox

A classic production puzzle is seeing a load average of 25.0 on a 4-core machine, but `top` reports CPU utilization at only 5%. Many engineers assume load average purely measures CPU demand. It does not.

#### What Load Average Actually Measures

Linux tracks an exponentially decaying moving average of the number of threads actively demanding hardware resources. Specifically, it counts tasks in two distinct scheduler states:

##### 1. State `R` (Running / Runnable):

- Threads currently on a CPU core executing instructions.

- Threads sitting in the run queue ready to execute the microsecond a core opens up.

##### 2. State `D` (Uninterruptible Sleep — usually Disk or NFS I/O):

- Threads waiting on hardware events (typically disk reads/writes, lock contention in kernel space, or a stalled network filesystem).

- Crucially, they cannot be interrupted by signals, not even `SIGKILL` (`kill -9`). The kernel refuses to preempt them because doing so mid-I/O could corrupt filesystem state or device driver structures.

#### The I/O Bottleneck Scenario

When you execute synchronous writes (like `dd` with direct sync flags), the process issues write syscalls that bypass or force flushes of the page cache directly to physical platters or flash blocks.

- The CPU issues the request to the storage controller via kernel space and immediately parks the thread in State `D` while awaiting hardware confirmation.

- Because the storage subsystem cannot commit the blocks fast enough, threads back up in state `D`.

- Result: Load Average climbs (tracking the count of stuck threads), while CPU usage stays near 0% because the CPU cores have literally nothing to compute—they are idle, marked under `%wa` (I/O wait).


### 3. Diagnostics: How the Subsystems Talk

When you run a 60-second triage pass, each command targets a specific layer in the OS execution path:

| **Layer**                      | **Primary CLI Tool** | **What is Happening Internally**                                                                                                                                                                                    |
| ------------------------------ | -------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Transport / Sockets**        | `ss -s`              | Queries kernel netlink socket tables. Shows open, established, and closed TCP sockets. High `TIME-WAIT` or `CLOSE-WAIT` flags port exhaustion or unclosed application connections.                                  |
| **Network Interface**          | `ip -s link`         | Pulls statistics directly from the network interface ring buffers. Displays interface drops (`dropped`) and transmission overruns (`overrun`), pointing to link congestion or driver bottlenecks.                   |
| **Virtual Memory & Scheduler** | `vmstat 1 3`         | Samples kernel counters over 1-second intervals. Tracks runnable processes (`r`), blocked processes (`b`), and critically: paging activity (`si`/`so` - swap in/swap out). Active swapping tanks latency instantly. |
| **Block Layer / Disk**         | `iostat -x 1 3`      | Reads `/proc/diskstats`. Highlights read/write throughput, average service times (`r_await`, `w_await`), and `%util` (how saturated the physical device is).                                                        |
| **Process Tree**               | `ps aux --sort=...`  | Traverses `/proc/[pid]/stat` to identify which exact processes are hoarding CPU cycles or physical memory (resident set size, `RSS`).                                                                               |

# 7-Day Roadmap: Networking, Linux Internals & Systems Engineering

> **Mission**: Build an intuitive, low-level mental model of what happens between the user space application, the OS kernel, and physical network packets.

> **Target Outcome**: Debug production issues, container networking bottlenecks, socket drops, and OS-level stalls with zero guesswork.

---

## 🛠️ Stack & Tooling Arsenal

Make sure you have a native Linux environment, WSL2 (Ubuntu 22.04+), or a dedicated Linux cloud VM ready.

| Category                        | Utilities & Technologies                                                                                                                                    |
| :------------------------------ | :---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Packet & Wire Inspection**    | `tcpdump`, `Wireshark`, `tshark`, `nc` (`netcat`), `curl`, `dig`, `nslookup`                                                                                |
| **Socket & Network State**      | `ss`, `ip` (`ip addr`, `ip route`, `ip link`), `netstat`, `ethtool`, `traceroute`, `mtr`                                                                    |
| **Kernel & Process Tracing**    | `strace`, `ltrace`, `ps`, `top`/`htop`, `pidof`, `pgrep`                                                                                                    |
| **Memory & Storage Telemetry**  | `/proc` filesystem (`/proc/[pid]/fd`, `/proc/meminfo`, `/proc/net/*`), `vmstat`, `free`, `lsof`                                                             |
| **Virtualization & Namespaces** | `ip netns`, `veth` pairs, `iptables`, `nftables`, Linux cgroups (v2)                                                                                        |
| **System Calls Under the Hood** | `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `epoll_create()`, `epoll_wait()`, `read()`, `write()`, `fork()`, `execve()`, `clone()`, `mmap()` |

---

## 📜 Daily Protocol & The "Paper Trail"

To ensure deep retention and create proof of work:

1. **Local Lab Repo**: Initialize a git repository named `systems-lab-zero-to-one`.
2. **Directory Structure**: Create folders `day-01/`, `day-02/`, etc.
3. **Daily Deliverable**: Every night before sleep:
    - Add your raw capture logs, command output snippets, and lab notes (`notes.md`).
    - Add any small experiments, test configs, or scratch files.
    - Run:
        ```bash
        git add .
        git commit -m "feat(day-XX): complete labs on <topic> [proof of work]"
        git push origin main
        ```

---

## 🗓️ Day-by-Day Curriculum

---

### 🟢 Day 1: Packets, The Wire & The 3-Way Handshake

#### Main Search Titles & Keywords

- `TCP 3-Way Handshake SYN SYN-ACK ACK deep dive`
- `tcpdump filter syntax cheat sheet`
- `Wireshark TCP sequence and acknowledgment numbers analysis`
- `TCP 4-Way Teardown FIN TIME_WAIT vs CLOSE_WAIT states`

#### Core Objectives

1. Understand how raw bytes traverse from user space to wire frames.
2. Demystify TCP flags: `SYN`, `ACK`, `FIN`, `RST`, `PSH`.
3. Capture, inspect, and decode real connection handshakes and terminations on live interfaces.

#### Hands-On Labs & Practice

- [x] **Lab 1.1: Live Packet Interception**
    - Run `tcpdump -nnvv -i any port 80 or port 443 -c 20` while making a curl request to `curl -v http://example.com`.
    - Save the traffic directly to a pcap file: `tcpdump -w day1_handshake.pcap port 80`.
- [x] **Lab 1.2: Frame Dissection**
    - Open `day1_handshake.pcap` in Wireshark or via `tshark -r day1_handshake.pcap -V`.
    - Locate the relative vs. absolute sequence numbers in the SYN, SYN-ACK, and ACK frames.
- [x] **Lab 1.3: Triggering an RST (Connection Reset)**
    - Use `nc` (Netcat) to attempt connecting to an unbound port on localhost (`nc -vz 127.0.0.1 9999`).
    - Capture the incoming `[RST, ACK]` packet with `tcpdump` and document why the kernel emitted it.

#### Paper Trail Task

- Export the filtered ASCII trace of the 3-way handshake and the RST packet into `day-01/handshake_analysis.md`.
- Write a 1-paragraph summary explaining why the `TIME_WAIT` state exists and what problem it solves.
- **Commit & Push.**

#### Optional Boss Fight (Stretch Goal)

- Induce a simulated packet drop or delay using `tc` (Traffic Control: `sudo tc qdisc add dev eth0 root netem delay 200ms`) and watch TCP window retransmissions (`TCP Retransmission` / `Dup ACK`) in Wireshark.

---

### 🟢 Day 2: The Socket Lifecycle, DNS & Modern Transports

#### Main Search Titles & Keywords

- `Berkeley sockets API lifecycle socket bind listen accept connect`
- `DNS resolution path dig trace root servers authoritative`
- `HTTP/1.1 vs HTTP/2 multiplexing wire format ALPN`
- `TLS 1.3 1-RTT handshake ClientHello ServerHello Certificate`

#### Core Objectives

1. Understand how the kernel maintains socket buffers and the connection backlog queue.
2. Trace DNS resolution hierarchically from root hints down to an A/AAAA record.
3. Observe real TLS 1.3 encryption negotiation and understand ALPN (Application-Layer Protocol Negotiation).

#### Hands-On Labs & Practice

- [x] **Lab 2.1: Berkeley Socket Mapping with `ss`**
    - Spin up a background listener: `nc -l -p 8080 &`.
    - Inspect socket states using `ss -tulpn`. Note the Recv-Q and Send-Q semantics for listening vs. connected sockets.
    - Connect a client: `nc 127.0.0.1 8080`. Observe the state change from `LISTEN` to `ESTAB`.
- [x] **Lab 2.2: Hierarchical DNS Walk**
    - Run `dig +trace www.microsoft.com`.
    - Trace the recursive query through the Root Servers (`.`), TLD Name Servers (`.com`), and authoritative DNS servers. Inspect the TTL and DNS record types (A, CNAME, NS).
- [x] **Lab 2.3: TLS 1.3 Handshake Sniffing**
    - Run `curl -Iv --tlsv1.3 https://www.cloudflare.com`.
    - Identify where the asymmetric key exchange (ECDHE) happens, when session keys are derived, and where application data starts showing up as encrypted TLS payloads (`Application Data Protocol: http/2`).

#### Paper Trail Task

- Create `day-02/socket_and_dns_notes.md`.
- Include the exact terminal output from `dig +trace` and `ss -tulpn`.
- Diagram the state transition of a socket from `LISTEN` -> `SYN_RCVD` -> `ESTABLISHED` -> `CLOSE_WAIT` -> `LAST_ACK` -> `CLOSED`.
- **Commit & Push.**

#### Optional Boss Fight (Stretch Goal)

- Decrypt TLS traffic in Wireshark locally by exporting the `SSLKEYLOGFILE` environment variable in your shell (`export SSLKEYLOGFILE=~/sslkeys.log`) and opening a browser or curl session.

---

### 🟢 Day 3: Linux System Calls & Tracing the Boundary

#### Main Search Titles & Keywords

- `Linux syscalls user space vs kernel space context switch`
- `strace tutorial tracing file open read write sockets`
- `Linux file descriptors stdin stdout stderr socket fd table`
- `lsof commands to find open files and network connections`

#### Core Objectives

1. Understand the exact line between user-space code and privileged kernel-space operations.
2. Master `strace` to diagnose hangs, permission errors, and unexpected file missing issues without touching source code.
3. Master Linux File Descriptors (`fd`), standard streams, and socket descriptors.

#### Hands-On Labs & Practice

- [x] **Lab 3.1: Reverse-Engineering Common Binaries with `strace`**
    - Run `strace -c ls /` to get a statistical profile of all system calls invoked.
    - Run `strace -e trace=open,openat,read,write cat /etc/hosts` to see every filesystem boundary crossed.
- [x] **Lab 3.2: Watching Network Syscalls Live**
    - Run `strace -e trace=network curl -s http://example.com > /dev/null`.
    - Document the exact sequence: `socket()` -> `connect()` -> `sendto()`/`write()` -> `recvfrom()`/`read()` -> `close()`.
- [x] **Lab 3.3: Inspecting File Descriptors via `/proc` and `lsof`**
    - Launch a python or bash process that sleeps: `sleep 1000 &` or open an interactive interpreter.
    - Navigate to `/proc/<PID>/fd/`. Inspect the symlinks (`0`, `1`, `2`).
    - Run `lsof -p <PID>` to see every shared library (`.so`) and pipe mapped to the process.

#### Paper Trail Task

- Save your annotated `strace` log of the `curl` network request to `day-03/syscall_network_trace.md`.
- Document 3 real debugging scenarios where `strace` immediately reveals bugs that compiler errors or stack traces cannot.
- **Commit & Push.**

#### Optional Boss Fight (Stretch Goal)

- [x] Attach `strace` to a running background process using the `-p <PID>` flag with timestamp tracking (`-tt`) and trace what it does when sent signals like `SIGTERM` and `SIGKILL`.

---

### 🟢 Day 4: Process Internals, Scheduling, Threads & Signals

#### Main Search Titles & Keywords

- `Linux process creation fork vs execve vs clone`
- `Process states R S D Z T uninterruptible sleep zombie processes`
- `POSIX signals SIGINT SIGTERM SIGKILL SIGSTOP SIGCONT handling`
- `Linux Completely Fair Scheduler CFS and thread priority`

#### Core Objectives

1. Understand how Linux processes and lightweight threads (LWP) are spawned and managed under `task_struct`.
2. Master process states, focusing heavily on state `D` (Uninterruptible Sleep / Disk I/O hang) and state `Z` (Zombie).
3. Understand signal dispatching and why `SIGKILL` (`9`) cannot be caught or ignored.

#### Hands-On Labs & Practice

- [x] **Lab 4.1: The Birth and Death of a Process**
    - Write a one-liner or simple script that forks a child and exits the child immediately, and parent runs without calling `wait()`, creating a temporary Zombie (`Z`) process.
    - Inspect it using `ps aux | grep 'Z'`. Verify what PID becomes the adopted parent (`init` / `systemd` PID 1).
- [x] **Lab 4.2: Investigating `/proc/[pid]/` Telemetry**
    - Pick an active process. Read `/proc/<pid>/status`, `/proc/<pid>/cmdline`, `/proc/<pid>/environ`, and `/proc/<pid>/limits`.
    - Check thread count: `ls /proc/<pid>/task/`. Compare process ID (PID) vs Thread Group ID (TGID).
- [ ] **Lab 4.3: Signal Trapping & Behavior**
    - Create a bash script that traps `SIGINT` (Ctrl+C) and `SIGTERM`, printing custom refusal messages.
    - Send signals using `kill -15 <pid>` and `kill -9 <pid>`. Verify that `SIGKILL` bypasses the user-space trap entirely and is executed immediately by the kernel.

#### Paper Trail Task

- Create `day-04/process_internals.md`.
- Document the exact steps to identify, kill, or clear a zombie process.
- Write down the architectural difference between a process and a thread from the Linux kernel's perspective (hint: `clone()` flags `CLONE_VM`, `CLONE_FS`, `CLONE_FILES`).
- **Commit & Push.**

#### Optional Boss Fight (Stretch Goal)

- Inspect CPU affinity and scheduling policies with `chrt` and `taskset`. Pin a CPU-intensive command to a single core (`taskset -c 0 stress ...`) and inspect per-core CPU utilization with `mpstat` or `htop`.

---

### 🟢 Day 5: Memory Virtualization, Page Faults & File Caching

#### Main Search Titles & Keywords

- `Virtual memory architecture page tables MMU TLB`
- `Page faults minor page fault vs major page fault disk read`
- `Linux VFS Virtual File System Page Cache Buffer Cache free vs available`
- `Linux OOM Killer Out Of Memory killer oom_score_adj`

#### Core Objectives

1. Understand virtual addresses vs. physical RAM and how the Memory Management Unit (MMU) translates them.
2. Dissect what actually happens during Major vs. Minor page faults.
3. Understand how the Linux kernel uses "free" RAM for disk Page Cache and why `free` memory is wasted memory.

#### Hands-On Labs & Practice

- [ ] **Lab 5.1: Decoding Memory Telemetry**
    - Run `free -h` and `vmstat 1 5`.
    - Read `/proc/meminfo` directly. Identify `MemAvailable`, `Buffers`, `Cached`, `Dirty`, and `SwapTotal`.
    - Write a short note in plain English clarifying the difference between `free` and `available`.
- [ ] **Lab 5.2: Tracking Page Faults**
    - Run `usr/bin/time -v <command>` (e.g. `/usr/bin/time -v ls -laR /usr`) to observe the output for **Major (requiring I/O) page faults** vs **Minor (reclaiming a frame) page faults**.
- [ ] **Lab 5.3: The Linux Page Cache in Action**
    - Generate a 500MB test file: `dd if=/dev/urandom of=testfile bs=1M count=500`.
    - Clear the system page cache: `sync; echo 3 | sudo tee /proc/sys/vm/drop_caches`.
    - Time the first read: `/usr/bin/time -v cat testfile > /dev/null`.
    - Time the second read immediately after: `/usr/bin/time -v cat testfile > /dev/null`.
    - Calculate the speedup and explain where the second read pulled the data from.

#### Paper Trail Task

- Create `day-05/memory_virtualization.md`.
- Record the benchmark numbers from Lab 5.3.
- Document how the Linux OOM Killer decides which process to terminate (`/proc/<pid>/oom_score` and `/proc/<pid>/oom_score_adj`).
- **Commit & Push.**

#### Optional Boss Fight (Stretch Goal)

- Examine a process's memory layout with `pmap -x <pid>` or inspect `/proc/<pid>/maps` to identify text, data, BSS, heap, memory-mapped regions (`mmap`), and stack segments.

---

### 🟢 Day 6: Container Plumbing — Namespaces, Cgroups & Virtual Networks

#### Main Search Titles & Keywords

- `Linux namespaces UTS PID NET MNT IPC USER from scratch`
- `Linux cgroups v2 resource limits memory cpu blkio`
- `Building a container from scratch using unshare and chroot`
- `Linux virtual network interface veth pair and bridge ip link`

#### Core Objectives

1. Demystify containers: understand that "containers are just ordinary Linux processes with namespace filters and cgroup limits".
2. Build an isolated container environment using core Linux tools without Docker or containerd.
3. Wire up two isolated network namespaces using a virtual ethernet (`veth`) pair.

#### Hands-On Labs & Practice

- [ ] **Lab 6.1: Namespace Isolation with `unshare`**
    - Create an isolated PID and hostname namespace:
        ```bash
        sudo unshare --fork --pid --net --mount-proc bash
        ```
    - Run `ps aux` inside this new shell. Notice how your bash shell is now **PID 1**.
    - Change the hostname inside using `hostname isolab`. Exit and confirm your host's hostname remained untouched.
- [ ] **Lab 6.2: Virtual Network Wiring (The Core of Docker/Kubernetes Networking)**
    - Create two distinct network namespaces:
        ```bash
        sudo ip netns add red
        sudo ip netns add blue
        ```
    - Create a virtual ethernet cable (`veth` pair):
        ```bash
        sudo ip link add veth-red type veth peer name veth-blue
        ```
    - Plug each end into the respective namespaces:
        ```bash
        sudo ip link set veth-red netns red
        sudo ip link set veth-blue netns blue
        ```
    - Assign IPs and bring interfaces up:
        ```bash
        sudo ip netns exec red ip addr add 10.0.0.1/24 dev veth-red
        sudo ip netns exec red ip link set veth-red up
        sudo ip netns exec blue ip addr add 10.0.0.2/24 dev veth-blue
        sudo ip netns exec blue ip link set veth-blue up
        ```
    - Test connectivity across the namespaces:
        ```bash
        sudo ip netns exec red ping -c 3 10.0.0.2
        ```
- [ ] **Lab 6.3: Enforcing Memory Limits via Cgroups v2**
    - Create a test cgroup: `sudo mkdir /sys/fs/cgroup/testgroup`.
    - Set a hard memory limit of 50MB: `echo "50M" | sudo tee /sys/fs/cgroup/testgroup/memory.max`.
    - Assign a process to it and monitor behavior when it hits the ceiling.

#### Paper Trail Task

- Create `day-06/container_internals_from_scratch.md`.
- Include the exact terminal commands and ping verification output from the virtual network namespace lab.
- Explain in 3 bullet points how Docker containers talk to the outside internet via a bridge interface (`docker0`) and NAT (`iptables` masquerade).
- **Commit & Push.**

#### Optional Boss Fight (Stretch Goal)

- Create a Linux Bridge (`sudo ip link add br0 type bridge`) and attach 3 different namespaces to it, simulating an internal software switch.

---

### 🟢 Day 7: The Capstone Incident — End-to-End Diagnostic War Room

#### Main Search Titles & Keywords

- `Linux performance troubleshooting methodology Brendan Gregg USE Method`
- `Debugging high load average but low CPU utilization disk I/O wait`
- `TCP connection timeouts dropped packets iptables firewall debugging`
- `Production debugging checklist for cloud engineers`

#### Core Objectives

1. Synthesize all networking, syscall, process, and memory knowledge into a unified, methodical debugging framework.
2. Formulate hypotheses and test them using non-destructive command-line diagnostics.
3. Write an executive engineering postmortem documenting root causes and preventative architectural remedies.

#### Hands-On Labs & Practice

- [ ] **Lab 7.1: The Mystery of High Load with Low CPU**
    - Understand the formula for Linux Load Average (R state + D state threads).
    - Simulate an artificial disk bottleneck using heavy sync writes (`dd if=/dev/zero of=hugefile bs=1M count=2000 oflag=dsync`) while checking `iostat -xz 1`, `vmstat 1`, and `top` (watching the `%wa` I/O wait column).
- [ ] **Lab 7.2: Diagnostic Gauntlet Execution**
    - Run a complete system health scan on your machine using only CLI tools in under 60 seconds:
        - Sockets: `ss -s` (socket summary)
        - Network bottlenecks: `ip -s link` (errors, dropped frames)
        - Memory pressure: `vmstat 1 3` (swapping, SI/SO)
        - I/O load: `iostat -x 1 3`
        - Top consumers: `ps aux --sort=-%cpu | head -10` and `ps aux --sort=-%mem | head -10`
- [ ] **Lab 7.3: Capstone Postmortem Documentation**
    - Select one high-profile real-world incident (e.g., Cloudflare 2019 ReDoS CPU spike, or an AWS Kinesis thread starvation incident).
    - Trace it down to the exact OS primitive: What syscall failed? What queue backed up? Why did the health check report healthy while user requests timed out?

#### Paper Trail Task

- Create `day-07/capstone_incident_report.md`.
- Formulate your personal 10-step "First Response" operational checklist for when a production service running in a Linux VM or container starts throwing HTTP 502/504 errors.
- Final commit:
    ```bash
    git add .
    git commit -m "feat(capstone): complete 7-day networking & linux internals boot camp"
    git push origin main
    ```

---

## 🏆 Checklist

Ensure you can comfortably explain without referencing docs:

- [ ] What exact network packets leave my machine when I run `curl https://azure.microsoft.com`?
- [ ] What is the difference between a process context switch and a thread context switch?
- [ ] Why does `kill -9` succeed when an application is locked in an infinite loop, but fails when stuck in state `D`?
- [ ] How does an epoll-based web server handle 100,000 concurrent open sockets without crashing the CPU?
- [ ] If two Docker containers on the same host cannot communicate, what 3 CLI commands do you run first to pinpoint the exact failure point?

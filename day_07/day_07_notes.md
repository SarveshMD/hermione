# Notes - Day 7

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

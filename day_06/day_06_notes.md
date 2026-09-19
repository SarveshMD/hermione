# Notes - Day 6

## Experiments

- Lab 6.1: Namespace Isolation with `unshare` - [terminal_1.md](terminal_1.md)

- Lab 6.2: Virtual Network Wiring - [terminal_2.md](terminal_2.md)

## My Notes

## Gemini Explanations

Containers are not virtual machines; there is no hypervisor or guest kernel running underneath. A container is simply a standard Linux process running directly on your host kernel, but constrained by two foundational Linux kernel features:

- Namespaces: Control what a process can see (its view of the system).

- Control Groups (cgroups): Control what a process can use (its resource consumption).

When you pair these two with a filesystem jail (like chroot or pivot_root), you get what the industry packages up and sells as a "container."

### The Big Three Prerequisite Concepts

#### 1. Linux Namespaces (The Illusion of Isolation)

By default, all processes on a Linux machine share a global view of system resources: the process table, network interfaces, mount points, and hostnames.

Namespaces wrap a global system resource into an abstraction so processes inside the namespace believe they have their own isolated instance of that resource.

- PID (Process ID): Provides an independent process tree. Inside a new PID namespace, your primary process becomes PID 1 (init-like behavior). It cannot see or signal processes running outside in the host's default namespace, though the host can still see it under a different PID.

- NET (Network): Provides an independent network stack: its own loopback interface, IP routing tables, firewall rules, and socket listings. An application bound to port 80 inside this namespace won't collide with port 80 on the host.

- MNT (Mount): Isolates the list of filesystem mount points. Mounting or unmounting a disk/pseudo-filesystem (like /proc) inside doesn't alter the host.

- UTS (UNIX Timesharing System): Isolates the hostname and domain name. You can name your container web-prod-01 without altering the host's actual hostname.

- IPC, USER, CGROUP: Isolate inter-process communication queues, user/group IDs (mapping a non-root host user to UID 0 root inside), and cgroup root view.

#### 2. Virtual Networking: The veth Pair

Because an isolated network namespace starts completely blank (only having a down lo interface), it has no way to talk to anything.

To bridge this, Linux provides a veth (Virtual Ethernet) pair. Think of it as a virtual, bidirectional Ethernet patch cord:

- Whatever packet enters one end immediately pops out of the other end.

- You leave one end pinned to one environment (e.g., host or another namespace) and physically shove the other end into the target namespace.

In Lab 6.2, you wire two namespaces directly together point-to-point. Later, when you want 10 or 50 containers talking to each other and the internet, you plug their host-side veth ends into a Linux software bridge (like docker0), which behaves just like a physical network switch.

#### 3. Cgroups v2 (Resource Walls)

While namespaces prevent a rogue process from seeing your host's processes or snooping on traffic, they do not prevent that process from consuming 100% of your RAM or CPU and locking up the box.

Control Groups (cgroups) meter and throttle resources:

- cgroups v2 organizes this via a unified hierarchy exposed as a pseudo-filesystem under /sys/fs/cgroup.

- Creating a cgroup is as simple as creating a directory (mkdir /sys/fs/cgroup/testgroup). The kernel automatically populates control files inside that folder.

- Writing a number into memory.max sets the ceiling. If processes inside exceed it and cannot free memory via reclaim, the kernel's Out-Of-Memory (OOM) Killer intervenes and terminates the offending process.

### Follow-ups

```text
is a container just a process in the main system/kernel with its own namespace, or is a namespace a outer covering over multiple processes, in which just one of them is the container process? are all processes running in a container a child of the container process that runs directly on the host itself ?

do we manually configure a process to get it's own namespace, or does all it's child processes just look at the parent (container process) like it's their entire world/namespace (pid 1), how does the other things like network, mount, uts, cgroup get assigned to the namespace?

veth pairs are interesting. they map a container's port to a) a host's port, b) a port / connection on the internet, c ) another container's port, d) a virtual switch like thing where multiple container's ports can be connected, etc,. right ? is this what I did when running docker run container_name -p sourcePort:destPort ? sourceport being the container's port, and destPort being the host system's port ?

cgroups, how does a /sys/fs/cgroup/new_cgroup know which container/namespace it belongs to ?
```

#### 1. Is a container just a process, or is a namespace an outer covering?

In the Linux kernel itself, the concept of a "container" does not exist. There is no struct container in kernel code.

A container is simply one or more standard Linux processes that have been assigned to private namespaces and cgroups.

- A namespace is not a process, nor is it a physical wrapper. It is an internal kernel bookkeeping structure. Every process descriptor in the kernel (struct task_struct) points to an nsproxy struct, which contains pointers to specific namespace instances (one for PID, one for NET, one for MNT, etc.).

- Multiple processes can point to the exact same namespace instance.

- When you start a container, you launch an entrypoint process inside new namespaces. If that entrypoint process forks background tasks, child workers, or helper scripts, all of those child processes inherit the exact same namespaces by default.

- Together, this collection of processes sharing those isolated namespaces is what we colloquially label "a container."

#### 2. Are all processes in a container children of the main container process on the host?

_Yes._

From the host kernel's perspective, the process tree is completely normal and hierarchical:

- The host kernel has its own init system (systemd, PID 1 on the host).

- Container runtimes (like containerd-shim or runc) fork off the initial process of your container (e.g., your Python app or web server).

- That initial process has a standard, real PID on the host (e.g., PID 41209).

- If your container process forks three worker processes, those workers will show up on the host as children of PID 41209 (e.g., PIDs 41210, 41211, 41212).

However, inside the container’s isolated PID namespace:

- PID 41209 appears as PID 1.

- The worker processes appear as PIDs 2, 3, and 4.

#### 3. How do processes get assigned to namespaces, and do child processes inherit them?

Processes get assigned to namespaces through explicit kernel system calls:

1. clone() with flags: When a parent creates a brand new child, it passes flags like CLONE_NEWPID, CLONE_NEWNET, CLONE_NEWNS. The kernel creates fresh namespace instances and places the new child directly inside them.

2. unshare(): A currently running process asks the kernel to detach it from its inherited namespaces and drop it into brand new ones. (This is what the CLI command unshare uses under the hood).

3. setns(): A process can attach itself to an existing namespace belonging to another running process (this is what docker exec does to drop your shell into an existing container).

#### 4. How do veth pairs work, and what is docker run -p actually doing?

A veth pair works at Layer 2 / Layer 3 (Ethernet/IP), not Layer 4 (ports).

A veth pair behaves like a virtual Ethernet patch cable. It has two ends, each acting as a virtual network interface card (NIC):

- End A: sits inside the container namespace (usually renamed eth0).

- End B: sits in the host's root namespace (usually named something like veth1234abc).

Your lettered breakdown is spot on conceptually:

- Point-to-point (a, c): You can plug End A into Container 1 and End B into Container 2 (or host) for direct communication.

- Virtual switch (d): In Docker or Kubernetes, End B is plugged into a virtual bridge (docker0 or cbr0). Multiple containers have their veth ends plugged into this bridge switch, allowing them to communicate on an internal private subnet (like 172.17.0.0/16).

##### What does docker run -p hostPort:containerPort actually do?

(Note: Docker's syntax is -p <hostPort>:<containerPort>, e.g., -p 8080:80 maps host port 8080 to container port 80).

The veth cable merely provides the pipe into the container's private IP (172.17.0.2). It does not bind the host's port by itself.

When you run -p 8080:80, Docker sets up Network Address Translation (NAT) via firewall rules (using iptables or nftables):

1. A rule is added: "If a packet arrives on the host targeting port 8080, rewrite the destination IP to 172.17.0.2 and destination port to 80."

2. The packet travels across the bridge (docker0), flows down the veth pipe, and arrives at the container's eth0.

3. The application listening inside the container on port 80 receives the request and replies back through the same veth tunnel.

#### 5. How does a cgroup know which container/namespace it belongs to?

Namespaces and cgroups do not inherently know about each other. They are completely separate subsystems in the kernel.

The link between them is the process PID itself:

- A cgroup directory (like /sys/fs/cgroup/testgroup) contains a control file named cgroup.procs.

- To put a process into a cgroup, you write its host PID into that file:

```bash
echo 41209 | sudo tee /sys/fs/cgroup/testgroup/cgroup.procs
```

- Once you write that PID, the kernel updates that process's internal descriptor so that all resource accounting (memory allocations, CPU cycles, disk I/O) is billed to testgroup.

- Because child processes inherit their parent's cgroup automatically on fork(), any process spawned inside that container gets tracked and bounded under the exact same cgroup.

Container engines like Docker simply coordinate the dance:

1. Create the cgroup directory under /sys/fs/cgroup/....

2. Set the resource limits (memory.max, cpu.max).

3. Spawn the container's root process inside new namespaces.

4. Take that process's host PID and write it into the cgroup's cgroup.procs file.

5. Plumb the veth interface into the process's network namespace.

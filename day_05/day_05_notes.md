# Notes - Day 5

## Experiments

- Lab 5.1: Decoding Memory Telemetry - [terminal_1.md](terminal_1.md)

- Lab 5.2: Tracking Page Faults - [terminal_2.md](terminal_2.md)

- Lab 5.3: Linux Page Cache in Action - [terminal_3.md](terminal_3.md)

## My Notes

1. Out-Of-Memory Killer
    - When the system runs out of memory, OOM Killer picks processes to kill with the highest OOM scores (`/proc/<pid>/oom_score`), so as to clear up memory and prevent kernel panic
    - Processes with uid 0 (`root`) get a reduction in thier `oom_score`
    - PID 1 (`init`/`systemd`) process is immune to avoid taking down the whole operating system
    - OOM Scores range from 0 and 1000. Highest score gets killed first.
    - The score depends directly on
        - Process Physical RAM (RSS - Resident Set Size) and Swap Used
    - `/proc/<pid>/oom_score_adj` holds adjustments for processes that need it. It ranges from `-1000` and `1000`
        - `0` is the default value. No adjustments
        - `-1000` -> Process is immune to OOM Killer, whatever the oom_score is.
        - `1000` -> Scapegoat -> kill this guy first in case of OOM, even if his oom_score is less.
    - Score Adjustment is applied in this manner:

        `adjusted_score = clamp(oom_score + oom_score_adj, 0, 1000)`

    - For some reason, when I look, almost all processes have an `oom_score` of around `666` to `670`. Gemini says it's a direct consequence of running inside WSL2.

## Gemini Explanations

Every running program lives inside an illusion. A process genuinely believes it owns a vast, flat, contiguous ocean of bytes starting at address zero, completely isolated from any other program running on the system.

In reality, physical RAM is a chaotic jigsaw puzzle. Your program’s neatly aligned memory is chopped up into tiny slices, scattered across non-contiguous hardware blocks, or not even in RAM at all yet.

### The Grand Illusion: Virtual vs. Physical Memory

If programs directly touched physical RAM addresses, two programs trying to write to address 0x1000 would overwrite each other’s data.

To solve this, modern hardware and operating systems divide memory into fixed-size chunks called pages (usually 4 KB on modern x86/ARM systems):

- Virtual Page: A 4 KB window in the program's perceived address space.

- Physical Frame (Page Frame): A real, physical 4 KB slot inside your motherboard's RAM chips.

The translation pipeline relies on three key players:

```text
        [ CPU executes instruction ]
                │
                ▼
        [ Virtual Address (Page # + Offset) ]
                │
                ▼
            ┌───────────┐
            │    TLB    │ ──(Hit: Instant physical address)──┐
            └─────┬─────┘                                    │
                │ (Miss: look in RAM)                      │
                ▼                                          │
            ┌───────────┐                                    │
            │Page Table │ ──(Walk multi-level table)─────────┤
            └─────┬─────┘                                    │
                │                                          │
                ▼                                          ▼
        [ Physical Frame # + Offset ] ────────────► [ Physical RAM ]
```

- MMU (Memory Management Unit): Dedicated silicon inside the CPU that intercepts every virtual memory access and translates it into a physical RAM address on the fly.

- Page Tables: In-memory data structures managed by the Linux kernel. They act as the master map: "Virtual Page X belongs to Physical Frame Y."

- TLB (Translation Lookaside Buffer): An ultra-fast hardware cache on the CPU die. Walking page tables in RAM is slow; the TLB caches recent translations. A TLB hit takes fractions of a nanosecond.

### Page Faults: Laziness as an Optimization

Linux almost never loads memory into RAM until the program literally tries to touch it. When your program asks for 1 GB via malloc(), Linux says "Sure, here's an address range," but allocates almost zero physical frames.

When the CPU tries to read or write an unmapped address, the MMU halts execution and throws an interrupt to the kernel: a Page Fault.

| Type             | What Triggers It                                                                                | What the Kernel Does                                                                                                                         | Cost                                      |
| ---------------- | ----------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------- |
| Minor Page Fault | The requested page is already in physical memory, just not mapped to this process's page table. | Finds the existing frame (e.g., shared libraries like `libc`, a newly allocated zeroed page, or a page already in the OS cache) and maps it. | Nanoseconds (CPU-only)                    |
| Major Page Fault | The data is not in RAM (it lives on disk or in swap).                                           | Halts the process, initiates disk/SSD I/O, copies data into a frame, updates page tables, and wakes the process.                             | Milliseconds (orders of magnitude slower) |

### The Page Cache: Why "Free" RAM is Wasted RAM

RAM costs power whether it holds zeroes or active data. Linux takes a pragmatic approach: unallocated RAM is doing nothing useful, while disk I/O is slow.

Whenever a file is read from or written to disk, the kernel stores those 4 KB blocks in unused RAM—this is the Page Cache.

- The first read of a file triggers a disk read and major page faults. The data lands in the Page Cache.

- The second read bypasses the disk entirely; the kernel hands back the data straight out of the RAM-based Page Cache.

- If a process suddenly demands memory for computation, the kernel instantly discards clean cached file pages to make room.

This brings us to the core distinction in telemetry:

- `free` memory: Completely untouched bytes. Doing absolutely nothing.

- `available` memory: True usable capacity (`free` + reclaimable Page Cache/Buffers) that the kernel can immediately reclaim without swapping.

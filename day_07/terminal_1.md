# Exercise 7.1 Terminal Outputs


## Stats before bottleneck

1. `vmstat 1` and `top`
- Current CPU Load Average: 0.03, 0.07, 0.04
- Current `%wa` (I/O wait): 0.00 in `top` and 0 in `vmstat`
- `vmstat`'s `b` (blocked processes) column: 0

   ![vmstat output](image.png)

   ![top output](image-1.png)

2. `iostat -xz 1`
- `%util` (device utilization): 0.80
- `w_wait` (write latency in ms): 2.33
- `aqu-sz` (average queue size): 0.01

    ![iostat output](image-2.png)

## Creating a bottleneck

```bash
dd if=/dev/urandom of=hugefile bs=1M count=2000 oflag=dsync
```

> **What `oflag=dsync` does**: Normally, writes go straight into the OS RAM (the page cache) and return instantly while the kernel flushes them lazily in the background. dsync forces every single 1 MB block to physically commit to storage hardware before the write syscall returns to user space.

## Stats during the bottleneck

1. `vmstat`
   - It caught the blocked state (`b` column)

2. `top`
   - Caught the `D` state (Uninterruptible Sleep) of `dd`

   ![bottleneck situation](image-3.png)

   ![top in bottleneck text](image-4.png)

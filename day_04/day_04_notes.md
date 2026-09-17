# Notes - Day 4

## Experiments

- Lab 4.1: Life cycle of a process - Hands-On with C programs - [terminal_1.md](terminal_1.md)
- Lab 4.2: Understanding Threads and some of the things in `/proc/<pid>/` - [terminal_2.md](terminal_2.md)
- Lab 4.3: Signals and Custom Signal handlers - C program - [terminal_3.md](terminal_3.md)

## My Notes

1. Process Internals
    - To identify and clear a zombie process
        - If we don't know it's PID, find it using `ps -eo pid,ppid,stat,cmd | grep ' Z'`
        - Either make the parent call `wait()`, or kill the parent so that the zombie is adopted by `systemd` (PID 1) and instantly called `wait()` on
    - Difference between a process and a thread from the Linux kernel's perspective
        - A process is created when `clone()` is called without the `CLONE_THREAD` flag resource sharing flags.
        - The child gets its own TGID, independent file descriptor table, etc,.
        - A thread inside the same process is created when `clone()` is called with the `CLONE_THREAD` flag and resource sharing flags so that the caller and the clone share virtual memory, file system and files and the clone gets a new PID.
        - `CLONE_THREAD` is the specific flag that tells the kernel that the cloned thread and the current process are going to share the same `TGID` (Thread Group ID) while giving the clone a different PID (TID).

## Gemini Explanations

At the heart of Linux execution sits a single unified structure: the `task_struct`.

To the Linux kernel, there is strictly no fundamental distinction between a "process" and a "thread." Both are simply schedulable entities represented by a `task_struct`. The difference we perceive in user-space comes entirely down to what resources are shared between these tasks.

### The Task Abstraction: Processes vs. Threads

When you create tasks, the kernel manages them using two primary identifiers:

- PID (Process ID in user-space): In kernel terms, this is actually the TGID (Thread Group ID). Every thread spawned by an application shares the same TGID, which is why `ps` or `top` reports them as having the same process ID.

- TID (Thread ID in user-space): In kernel terms, this is the actual unique PID assigned to that specific `task_struct`.

When a single-threaded program launches, its PID equals its TGID. When it spawns lightweight threads, each new thread gets its own unique kernel PID, but inherits the parent's TGID. You can see this directly inside `/proc/<pid>/task/`, where each folder represents an individual thread's kernel PID.

### The State Machine: R, S, D, Z, and T

A task moves across discrete lifecycle states during scheduling:

| State | Name                  | Trigger & Behavior                                                                                                                                                                                             |
| ----- | --------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| R     | Running / Runnable    | Actively executing on a CPU core or queued in the Completely Fair Scheduler (CFS) runqueue waiting for a timeslice.                                                                                            |
| S     | Interruptible Sleep   | Waiting for an event, timer, socket read, or lock. It wakes up if an event occurs or if any signal is delivered.                                                                                               |
| D     | Uninterruptible Sleep | Waiting directly on hardware, typically synchronous disk I/O, NFS locks, or kernel device drivers. The kernel will not wake this task for signals (not even `SIGKILL`) to prevent driver or device corruption. |
| T     | Stopped / Traced      | Suspended by job control (like pressing `Ctrl+Z` sending `SIGTSTP`, or debugger tracing). Resumes only upon receiving `SIGCONT`.                                                                               |
| Z     | Zombie                | The task finished execution, freed its memory, stack, and file descriptors, but its entry in the kernel process table remains. It stays here until its parent reads its exit status.                           |

### Why Zombies Exist (and Why You Can't Kill Them)

When a child task dies, the kernel preserves its metadata—exit code and resource stats—in a `task_struct` so the parent can inspect it using the `wait()` or `waitpid()` syscall.

Because a zombie is already dead, sending `kill -9` does nothing. There is no memory, no code, and no instruction pointer to terminate. A zombie only disappears when:

1. The parent calls `wait()` to harvest the exit code (reaping).

2. The parent dies. When a parent dies before reaping, the child becomes an orphan. The kernel immediately re-parents the orphan to PID 1 (`systemd` / `init`), which continuously runs `wait()` cycles to automatically sweep and reap dead children.

### Signal Architecture & The `SIGKILL` Exception

Signals are asynchronous software interrupts sent to a task. When a signal is sent:

1. The kernel registers a pending bit in the task's `task_struct`.

2. As the task switches context from kernel mode back to user mode, the kernel checks pending signals.

3. If a user-space signal handler is registered (via sigaction), the kernel adjusts the user stack to execute the custom handler before returning to normal execution.

- `SIGTERM` (15): A polite request to terminate. The application can catch it, flush write buffers, close database connections, and shut down cleanly.

- `SIGINT` (2): Sent via terminal interrupt (`Ctrl+C`). It can also be trapped, overridden, or ignored.

- `SIGKILL` (9) & `SIGSTOP` (19): Cannot be caught, blocked, or ignored. The kernel handles them directly inside the dispatcher. When the pending `SIGKILL` bit is evaluated, the kernel destroys the task's execution context immediately without transferring control to user-space.

# Exercise 4.2 Terminal Outputs

## Demo with `zombie_lab_wait`

- Screenshot of parent `zombie_lab_wait` process's status before it was done
  ![parent status](images/image-2.png)

- Screenshot of child `zombie_lab_wait` process's status before parent called `wait()`
  ![before wait() call](images/image-3.png)
- Even thought the left pane says the parent is done, the `cat /proc/<childPID>/status` command was run before `wait()` call

- After the wait() call, `/proc/<childPID>/` directory was gone

## More `/proc/<pid>/` files

- `cmdline`, `environ`, `limits`
  ![sleep and it's parent](images/image-4.png)
  ![cmdline, environ, limits](images/image-5.png)

## IMPORTANT NOTE: FORK DOES NOT CREATE A THREAD. IT CREATES A CHILD PROCESS

- So the child doesn't get an entry in `/proc/<parentPID>/task/`
- I was trying to find `<childPID>` in `/proc/<parentPID>/task` while running `zombie_lab_wait` only to learn this now.

## Threading

- `threads.py` creates a thread and asks it to sleep for 5 minutes, and goes to sleep for 5 minutes

- Created thread is visible:

    ![Thread visible in /proc/pid/task](images/image-7.png)

### Status Comparison

|         Python Main Process (`/proc/<pid>/status`)         |           Created Thread (`/proc/<threadPID>/status`)           |
| :--------------------------------------------------------: | :-------------------------------------------------------------: |
| ![Python Program's /proc/<pid>/status](images/image-6.png) | ![/proc/<pid>/status of the created thread](images/image-8.png) |
|               _Main process status metrics_                |                 _Worker thread status metrics_                  |

### Notes

- They have the same TGID and different PID
- Both are instances of `python3` program
- `Threads: 2` - two threads on the same thread group
- Both `status` outputs show identical memory footprints: `VmRSS`, `VmSize`, `VmStk`

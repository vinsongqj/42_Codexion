# Codexion

*This project has been created as part of the 42 curriculum by vgoh*
## Description
### Overview

Codexion is a C program designed to simulate concurrent resource contention in a multi-threaded environment. It demonstrates the challenges of process synchronization, deadlock avoidance, and real-time scheduling in systems where multiple "coders" (threads) compete for limited "dongles" (shared resources).

The program uses a scheduler to manage thread lifecycle and prevent starvation, ensuring that all coders can execute their tasks within defined time constraints.

The program takes the following arguments:
1. `number_of_coders`: The number of coders and also the number of dongles.

2. `time_to_burnout` (in milliseconds): If a coder did not start compiling within the specified time since the beginning of their last compile or the
beginning of the simulation, they burn out.

3. `time_to_compile` (in milliseconds): The time it takes for a coder to compile.
During that time, they must hold two dongles.

4. `time_to_debug` (in milliseconds): The time a coder will spend debugging.

5. `time_to_refactor` (in milliseconds): The time a coder will spend refactoring.
After completing the refactoring phase, the coder will immediately attempt to
acquire dongles and start compiling again.

6. `number_of_compiles_required`: If all coders have compiled at least this
many times, the simulation stops. Otherwise, it stops when a coder burns
out.

7. `dongle_cooldown` (in milliseconds): After being released, a dongle is unavailable until its cooldown has passed.

8. `scheduler`: The arbitration policy used by dongles to decide who gets them
when multiple coders request them.
The value must be exactly one of: fifo or edf.
fifo means First In, First Out: the dongle is granted to the coder whose
request arrived first.
edf means Earliest Deadline First with deadline = last_compile_start +
time_to_burnout.

### Blocking Cases Handled

* **Deadlock Prevention:** The "Circular Wait" condition is addressed by enforcing resource ordering. Coders always grab the lower-indexed dongle before the higher-indexed one.

* **Starvation Prevention:** By utilizing an EDF-based priority heap, coders closest to the burnout threshold are prioritized, preventing any single coder from being indefinitely denied dongles.

* **Log Serialization:** Console output is serialized via a global mutex, preventing jumbled or overlapping logs during high-concurrency periods.

* **Precise Burnout Detection:** To catch burnouts, each thread tracks the timestamp of its last successful compile, and `monitor_routine` checks these against the current time every 10ms to ensure a burnout is detected and the simulation ends.

### Thread Synchronization Mechanisms

* `pthread_mutex_t`: Used to enforce mutual exclusion on the shared table state, dongle locks, and log serialization. This ensures that even with massive contention, the simulation state remains consistent.

* **Atomic State Updates:** By wrapping the     `compiles_done` and `last_compile` updates in a `stop_lock` mutex, race conditions are prevented when retrieving the data.

* **Custom Scheduling Logic:** Priority logic and burnout monitoring are managed within `scheduler.c`, which uses an EDF-based queue to prioritize coders close to burnout.

## Instructions
A Makefile has been created to simplify the compilation of the program, after cloning the repository,  you may run these commands:
```
make           # compiles the program
make debug     # debugs the program with fsanitize
make clean     # removes all object files
make fclean    # removes all build files
make re        # removes all build files and recompiles the program
```
You may also choose to compile the program manually:
```
gcc -Wall -Wextra -Werror -pthread -Iinclude src/*.c -o codexion
```

After compiling you can run the program by inputting several parameters:
```
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```
| Parameters  | Type | Input Constraints |
| ------------- |:-------------:|:-------------:|
| number_of_coders      | int    | > 0 |
| time_to_burnout      | int    | > 0 (in milliseconds)|
| time_to_compile      | int | > 0 (in milliseconds)  |
| time_to_debug      | int | > 0 (in milliseconds) |
| time_to_refactor     | int | > 0 (in milliseconds) |
| number_of_compiles_required | int | >= 0 |
| dongle_cooldown     | int | >= 0 (in milliseconds) |
| scheduler      | string | fifo / edf |

Example input:
```
./codexion 5 800 200 150 100 10 50 edf
```

## Resources

* [CodeVault - Unix Threads in C](https://youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&si=45gooKHRAwXUAIKu)

* [Jacob Sorber - Programming with Threads](https://youtube.com/playlist?list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&si=0Wf8-dQyKEno1L4A)

* [Core Dumped - Why Are Threads Needed On Single Core Processors ](https://youtu.be/M9HHWFp84f0?si=Q-8ds__QUXaeh0_u)

* [Core Dumped - Threads on Multicore Systems](https://youtu.be/5sw9XJokAqw?si=-5MvSP9_QHU_jFqN)

* [Core Dumped - What Happens When A Program Calls Sleeps](https://youtu.be/e5g8eYKEhMw?si=ETsq_mChXmNInpQW)
## Disclosure of AI Use
Gemini was used to help with finding edge cases, error handling, optimization, structuring the readme and furthering my understanding about the topic.

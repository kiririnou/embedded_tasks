# Logic analyzer

### Recorded button pushes
| Push id | Bounce duration | Impulses captured by logic analyzer | Impulses captured by MCU |
|----------|----------|----------|----------|
|  1 | not caught | 1 | 2 |
|  2 | not caught | 1 | 1 |
|  3 | not caught | 1 | 1 |
|  4 | not caught | 1 | 2 |
|  5 | not caught | 1 | 1 |
|  6 | not caught | 1 | 2 |
|  7 | not caught | 1 | 2 |
|  8 | 57.9 microseconds | 2 | 2 |
|  9 | not caught | 1 | 1 |
| 10 | not caught | 1 | 1 |


Q&A
1. Is the number of impulses equivalent between MCU and logic analyzer?
    No
2. In which cases MCU "misses" bouncing (if it happens), and when counts more?
    MCU maybe skipping some of the signal bouncing due to it's internal sampling and if signal is too short.
    It may be counting more button presses due to button not being perfect and having some internal bouncing of physical contacts.
    Logic analyzer also maybe skipping some of the signal bouncing due to sampling rate.
3. Average time of button bouncing
    ~60 microseconds
4. What minimal debounce time could be enough?
    Probably around 30 ms to filter out most of the noise around.

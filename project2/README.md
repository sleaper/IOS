# Project Overview: Synchronization with Skibus System
This project simulates a skibus system involving three process types: a main process, the skibus, and skiers. After breakfast, each skier waits at a designated skibus stop, boarding upon the bus's arrival. The skibus travels between stops, accommodating skiers based on its capacity until all are delivered to a ski lift station.

# How to Run

```bash
$ ./proj2 L Z K TL TB
```
- **L**: Number of skiers (max 20,000)
- **Z**: Number of stops (1 to 10)
- **K**: Capacity of the skibus (10 to 100)
- **TL**: Maximum time a skier waits at a stop (0 to 10,000 microseconds)
- **TB**: Maximum travel time between stops for the skibus (0 to 1,000 microseconds)

## Key Features
- **Processes**: Runs a total of 2+L processes including the main process, skibus, and L skiers.
- **Synchronization**: Utilizes semaphores and shared memory for synchronizing actions without active waiting.
- **Output**: All actions are logged in sequence in a file named proj2.out.

## Compilation and Delivery
- Use the make command in the directory containing the Makefile to compile.
- The resulting executable, proj2, will only rely on system libraries.

## Testing and Output
Example output for a run command can be seen in the proj2.out file, showcasing the process flow from skiers starting, boarding the bus, and going skiing.

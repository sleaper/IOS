#!/bin/bash

# Number of times to execute the program
NUM_EXECUTIONS=20

# Timeout value in seconds
TIMEOUT=10

# Loop to execute the program NUM_EXECUTIONS times
for ((i=1; i<=NUM_EXECUTIONS; i++))
do
    # Generate random values for each parameter
    L=$((RANDOM % 20000 + 1))  # Random number between 1 and 20000
    Z=$((RANDOM % 10 + 1))     # Random number between 1 and 10
    K=$((RANDOM % 91 + 10))    # Random number between 10 and 100
    TL=$((RANDOM % 10001))     # Random number between 0 and 10000
    TB=$((RANDOM % 1001))      # Random number between 0 and 1000

    # Specify the path to the program and pass random numbers as arguments
    PROGRAM="./proj2 $L $Z $K $TL $TB"

    echo "Executing program iteration $i with random numbers: L=$L, Z=$Z, K=$K, TL=$TL, TB=$TB"
    timeout $TIMEOUT $PROGRAM
    if [ $? -eq 124 ]; then
        echo "Program timed out"
        exit 1
    fi
done

echo "Execution of program completed"

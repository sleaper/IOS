#!/bin/bash

# Number of times to execute the program
NUM_EXECUTIONS=20

# Timeout value in seconds
TIMEOUT=10

# Loop to execute the program NUM_EXECUTIONS times
for ((i=1; i<=NUM_EXECUTIONS; i++))
do
    # Generate random values for each parameter
    L=$(shuf -i 1-19999 -n 1)  # Random number between 1 and 20000
    Z=$(shuf -i 1-10 -n 1)     # Random number between 1 and 10
    K=$(shuf -i 10-100 -n 1)    # Random number between 10 and 100
    TL=$(shuf -i 0-10000 -n 1)     # Random number between 0 and 10000
    TB=$(shuf -i 0-1000 -n 1)      # Random number between 0 and 1000

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

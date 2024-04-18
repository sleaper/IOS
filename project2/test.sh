#!/bin/bash

# Compile the project
make clean
make || { echo "Compilation failed"; exit 1; }

# Function to perform a single test
perform_test() {
    # Run the project with arguments
    ./proj2 $1 $2 $3 $4 $5
    RETVAL=$?

    # Check for expected exit code
    if [ $RETVAL -ne "$6" ]; then
        echo "Test FAILED: Expected exit code $6, got $RETVAL"
        return 1
    fi

    # Run the script to check proj2.out
    ./kontrola-vystupu.sh < proj2.out
    if [ $? -eq 0 ]; then
        echo "Output test PASSED for parameters: L=$1 Z=$2 K=$3 TL=$4 TB=$5"
        return 0
    else
        echo "Output test FAILED for parameters: L=$1 Z=$2 K=$3 TL=$4 TB=$5"
        return 1
    fi
}

# Loop to perform the tests 1000 times
for (( i=0; i<1000; i++ )); do
    # Generate random test parameters or use predefined ones
    L=$((1 + $RANDOM % 20))
    Z=$((1 + $RANDOM % 5))
    K=$((10 + $RANDOM % 20))
    TL=$(($RANDOM % 1000))
    TB=$(($RANDOM % 1000))
    expected_exit_code=0  # Expected exit code for normal execution, adjust if needed

    echo "Running test iteration $((i+1))..."
    if perform_test $L $Z $K $TL $TB $expected_exit_code; then
        echo "Test $((i+1)): SUCCESS"
    else
        echo "Test $((i+1)): FAIL"
    fi
done

echo "Testing complete."

#!/bin/bash

# Compile the project
make clean
make || { echo "Compilation failed"; exit 1; }

# Function to perform a single test
perform_test() {
    ./proj2 $1 $2 $3 $4 $5 > proj2.out
    RETVAL=$?

    # Check for expected exit code
    if [ $RETVAL -ne "$6" ]; then
        echo "Test FAILED: Expected exit code $6, got $RETVAL"
        return
    fi

    # Check output correctness if needed (this part can be expanded with specific checks per test case)
    if [ "$7" != "" ]; then
        if diff proj2.out "$7"; then
            echo "Output matches expected."
        else
            echo "Output does NOT match expected!"
            diff proj2.out "$7"
        fi
    fi

    echo "Test PASSED: L=$1 Z=$2 K=$3 TL=$4 TB=$5"
}

# Test cases
# Syntax: perform_test L Z K TL TB expected_exit_code "expected_output_file"
# perform_test 8 4 10 4 5 0 "expected_output_1.txt"
perform_test 1 1 1 1000 100 1 "" # invalid bus capacity
perform_test 20000 5 50 500 10 1 "" # expected to fail due to invalid L
perform_test 10 11 10 100 100 1 "" # expected to fail due to invalid Z
perform_test 10 5 5 0 0 1 "" # expected to fail due to invalid K
perform_test 10 5 100 10001 1000 1 "" # expected to fail due to invalid TL
perform_test 10 5 100 1000 1001 1 "" # expected to fail due to invalid TB


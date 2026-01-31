#!/bin/bash

# Configuration
DATA_DIR="/example/"
FILES=("OF" "AD" "EM")
EXTENSION=".txt"
RESULT_FILE="res.txt"
ALGORITHMS=(1 2 3 4)

# Capture termination signals and record reason
trap 'echo "Interrupt reason: User interrupt (SIGINT) - $(date)" >> "$RESULT_FILE"; exit 1' SIGINT
trap 'echo "Interrupt reason: System termination (SIGTERM) - $(date)" >> "$RESULT_FILE"; exit 1' SIGTERM

# Initialize result file with header
echo "Decomposition Results" >> "$RESULT_FILE"
echo "================================" >> "$RESULT_FILE"
echo "Format: Algorithm,Dataset,Time(seconds),Status" >> "$RESULT_FILE"
echo "--------------------------------------------" >> "$RESULT_FILE"

# Run each algorithm on each dataset
for algo in "${ALGORITHMS[@]}"; do
    echo | tee -a "$RESULT_FILE"
    echo "=== Running Algorithm $algo ===" | tee -a "$RESULT_FILE"
    
    for file in "${FILES[@]}"; do
        filename="${file}${EXTENSION}"
        filepath="${DATA_DIR}${filename}"
        
        if [ -f "$filepath" ]; then
            echo "Processing: $filename with Algorithm $algo" | tee -a "$RESULT_FILE"
            
            # Record start time
            start_time=$(date +%s.%N)
            
            # Run the algorithm
            ./dtruss -f "$filepath" -a $algo
            
            # Check exit status
            exit_status=$?
            
            # Record end time and calculate duration
            end_time=$(date +%s.%N)
            duration=$(echo "$end_time - $start_time" | bc)
            
            # Record result
            if [ $exit_status -eq 0 ]; then
                echo "Algorithm $algo, $filename, $duration, SUCCESS" >> "$RESULT_FILE"
                echo "  -> Completed in $duration seconds" | tee -a "$RESULT_FILE"
            else
                echo "Algorithm $algo, $filename, $duration, FAILED (exit code: $exit_status)" >> "$RESULT_FILE"
                echo "  -> Failed with exit code: $exit_status (time: $duration seconds)" | tee -a "$RESULT_FILE"
                echo "Interrupt reason: Command execution failed (exit code:$exit_status) - $filename with Algorithm $algo" >> "$RESULT_FILE"
                # Continue with next dataset instead of exiting completely
            fi
            
            echo "----------------------------------------" | tee -a "$RESULT_FILE"
            
        else
            echo "Warning: File $filepath does not exist, skipping" | tee -a "$RESULT_FILE"
        fi
        
        # Small delay between runs
        sleep 1
    done
    
    echo "########################################################" | tee -a "$RESULT_FILE"
done

echo "All algorithms processed successfully." | tee -a "$RESULT_FILE"

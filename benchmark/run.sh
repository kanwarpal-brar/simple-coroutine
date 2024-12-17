#!/bin/bash

# Create the results directory if it doesn't exist
mkdir -p ./results

# Loop through all the scripts in the ./executables folder
for script in ./executables/*; do
    if [[ -x "$script" ]]; then
        timestamp=$(date +%s)
        result_file="./results/result_${timestamp}.out"
        echo "====================================" >> "$result_file"
        echo "Running $script..." >> "$result_file"
        echo "====================================" >> "$result_file"
        { time "$script"; } &>> "$result_file"
        echo -e "\n\n" >> "$result_file"
    else
        echo "$script is not executable" >> "$result_file"
    fi
done
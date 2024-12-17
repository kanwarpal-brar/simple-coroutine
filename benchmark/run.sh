#!/bin/bash

# Directories
results_dir="/workspaces/simple-coroutine/benchmark/results"
executables_dir="/workspaces/simple-coroutine/benchmark/executables"

# Create the results directory if it doesn't exist
mkdir -p $results_dir
mkdir -p $executables_dir

echo "start"
# Make all files in executables folder executable
find $executables_dir -type f -exec chmod +x {} \;

# Loop through all the scripts in the ./executables folder
timestamp=$(date +%s)
result_file="$results_dir/result_${timestamp}.out"
for script in $executables_dir/*; do
    if [[ -x "$script" ]]; then
        echo "Running $script"
        echo "Running $script" >> "$result_file"
        echo "====================================" >> "$result_file"
        { time "$script" > /dev/null; } &>> "$result_file"
        echo -e "\n" >> "$result_file"
    else
        echo "$script is not executable" >> "$result_file"
    fi
done
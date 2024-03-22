#!/bin/bash

# Define the folder containing your JSON files
JSON_FOLDER="../assets"

# Check if the folder exists
if [ ! -d "$JSON_FOLDER" ]; then
    echo "Error: JSON folder not found."
    exit 1
fi

# Path to your executable
EXECUTABLE="./raytracer"

# Loop through each JSON file in the folder
for JSON_FILE in "$JSON_FOLDER"/*.json; do
    if [ -f "$JSON_FILE" ]; then
        echo "Running $EXECUTABLE with JSON file: $JSON_FILE"
        $EXECUTABLE "$JSON_FILE"
        echo "Execution completed for JSON file: $JSON_FILE"
    fi
done

#!/bin/bash

make

# Generate a random port number between 1024 and 49151
RANDOM_PORT=$(shuf -i 1024-65535 -n 1)
# Start the server on the random port
cd server
./server.exe $RANDOM_PORT &
cd ..
# Wait for a brief moment to ensure the server is up and running
sleep 1

# Run the deliver process with the same random port
yes ftp message.txt | head -n 2| ./deliver/deliver.exe localhost $RANDOM_PORT 
sleep 5
yes ftp demo.png | head -n 2| ./deliver/deliver.exe localhost $RANDOM_PORT 

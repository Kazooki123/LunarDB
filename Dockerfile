FROM ubuntu:latest

# Set environment variables to avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && \
    apt-get install -y g++ make liblua5.3-dev

# Set the working directory inside the container
WORKDIR /usr/src/lunardb

# Copy all files from the current directory into the container
COPY . .

# Compile LunarDB in a single RUN command to improve layer efficiency
RUN g++ -std=c++17 -o bin/lunar src/main.cpp src/cache.cpp src/saved.cpp src/sql.cpp src/module.cpp src/hashing.cpp && \
    chmod +x bin/lunar

# Command to run LunarDB when the container starts
CMD ["./bin/lunar"]

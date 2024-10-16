FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y g++ make liblua5.3-dev

WORKDIR D:/cpp_database_cache/lunar/

COPY . .

RUN g++ -std=c++17 -o bin/lunar main.cpp cache.cpp saved.cpp sql.cpp module.cpp hashing.cpp \
    && chmod +x bin/lunar

# Expose a port if your app serves a network interface
# EXPOSE 8080

# Command to run LunarDB when the container starts
CMD ["./bin/lunar"]

# FULL COMPILE GUIDE FOR LUNARDB IN LINUX

Here, we'll be diving into the full documentation of compiling
LunarDB in multiple Linux Distros!

## Note

By default, LunarDB is currently compiled in Ubuntu, files such as `main.cpp`, `core_MAIN.cpp` & `core_MAIN.h`

## Ubuntu / Debian

Note:

this works for any Debian-based or Ubuntu-based Operating Systems.

Step 1:

```bash
sudo apt update
sudo apt install build-essential liblua5.4-dev libpqxx-dev libpq-dev libcurl4-openssl-dev libboost-all-dev libasio-dev
```

Step 2:

Navigate to `src/` and then run this, you don't need to include every file
in the source directory as most are barely finished.

```bash
g++ -std=c++17 main.cpp core_MAIN.cpp cache.cpp connect.cpp concurrency.cpp saved.cpp sql.cpp module.cpp parser.cpp sharding.cpp hashing.cpp -I/usr/include/lua5.4 -llua5.4 -lpq -lcurl -lboost_system -pthread -o ../bin/lunardb
```

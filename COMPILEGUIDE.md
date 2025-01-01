# FULL COMPILE GUIDE FOR LUNARDB IN LINUX

Here, we'll be diving into the full documentation of compiling
LunarDB in multiple Linux Distros!

## Ubuntu / Debian

## Note:

```
this works for ny Debian-based or Ubuntu-based Operating Systems.
```

Step 1:

```bash
sudo apt update
sudo apt install build-essential liblua5.4-dev libpqxx-dev libpq-dev libcurl4-openssl-dev libboost-all-dev libasio-dev
```

Step 2:

Navigate to `src/` and then run this, you don't need to include every file
in the source directory as most are barely finished.

```bash
g++ -std=c++17 main.cpp core.cpp cache.cpp connect.cpp concurrency.cpp saved.cpp sql.cpp module.cpp parser.cpp sharding.cpp hashing.cpp -I/usr/include/lua5.4 -llua5.4 -lpqxx -lpq -lcurl -lboost_system -pthread -o ../bin/lunardb
```

# Arch

## Note:

```
This works at any Arch-based Linux distros (e.g. Endeavour, Manjaro)

Depending on which Arch-based you're using, in vanilla Arch, you'd need to install
specific dependencies from scratch like Lua, curl, boost, gcc/g++ and PostgreSQL.

But for Endeavour OS, lua, curl, and gcc are come preinstalled.
```

Step 1:

Update Arch

```bash
sudo pacman -Syu
```

Step 2:

Install dependencies for compiling LunarDB in Arch.

```bash
sudo pacman -S lua postgresql-libs curl boost gcc
```

Step 3:

Compile

```bash
g++ -std=c++17 main.cpp core.cpp cache.cpp connect.cpp concurrency.cpp saved.cpp sql.cpp module.cpp parser.cpp sharding.cpp hashing.cpp -I/usr/include/lua -lllua -lpq -lcurl -lboost_system -pthread -o ../bin/lunardb
```

Got a problem or issue with this documentation? Please make a PR **:)**
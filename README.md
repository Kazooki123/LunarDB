<div align="center">
  <img src="lunardblogomain.png" alt="LunarDB">

  <h1>🌑LunarDB: A cache key-value database</h1>

  <h3><b>⚡A Blazingly Fast Cache database⚡</b></h3>
</div>

## 🔗What is it?

LunarDB is a **multi-model, key-value store, cache, memory based database** that loads keys and values in a file when saved by command, you can load this keys and values by the command "LOAD" then filename.

## Install

A Guide to **Installing LunarDB**!

### 💽Git pull (Recommended)

If you want the full source code of **LunarDB** and **compile** it for yourself then you can run:

```bash
git pull https://github.com/Kazooki123/lunardb.git
```

### 🪟Windows

While **outdated**, you can go to the `installer/` directory and get yourself a **LunarDB CLI executable** file!

### 🐧Linux

`Coming soon...`

### 🍎Apple

You can use and install **Homebrew** and run this:

```bash
brew install lunar
```

## 📖Documentation

**💫LunarDB** has a documentation website with a guide for users who are new and learning to use LunarDB!

**🌐Visit** [here](https://lunardbdocs.vercel.app/docs/)

## 🔗Commands

**🧭Basic commands:**

SET key value [ttl] - Set a key-value pair with optional TTL in seconds.

GET key - Get the value for a key.

DEL key - Delete a key-value pair.

MSET key1 value1 key2 value2 ... - Set multiple key-value pairs

MGET key1 key2 ... - Get multiple values

KEYS - List all keys

HASH SHA256 key - Hashes a key using SHA-256

HASH MURMUR3 key - Hashes a key using the MURMUR3 encryption method

HASH ROTATE input shift - Rotates a hash base on the amount of shifts

CLEAR - Clear all key-value pairs

SIZE - Get the number of key-value pairs

CLEANUP - Remove expired entries

SAVE filename - Save the cache to a file

LOAD filename - Load the cache from a file

LPUSH key value - Push an element to the head of the list

LPOP key - Remove and return an element from the head of the list

RPUSH key value - Push an element to the tail of the list

RPOP key - Remove and return an element from the tail of the list

LRANGE key start stop - Get a range of elements from the list

LLEN key - Get the length of the list

QUIT - Exit the program

### Additional Notes

For more commands you can run the **CLI** executable or read our **Documentation Page**.

## Docker

A **Guide** how to use **LunarDB** using Docker!

### Pull

First, pull the lunardb docker image:

```bash
docker pull kazookilovescoding/lunardb
```

### Run it

For running the lunardb docker image:

```bash
docker run kazookilovescoding/lunardb
```

And you're done! You should have the **LunarDB** Docker Image in your machine now!

## 🫂Community

**Join** the LunarDB Community!

- [Discord](https://discord.gg/KgxNzAZ5)

## ⚙️Compiling LunarDB

When it comes to Compiling **LunarDB**, there are few methods to compile it
Currently, We use **G++/GCC** for compiling C++ files and then output them as executables!

### Important Note

**Note:** You would need:

- The Source codes for **Lua v5.4.4**
- For Linux this would be `liblua5.4-dev` (Ubuntu) and `lua-dev` (Arch)

### 🪟Compiling for Windows

**⚠️ LUNARDB IS CURRENTLY ON SUPPORT FOR LINUX FOR NOW SO COMPILING MIGHT BROKE THE MAIN WINDOWS EXE FILE AS `main.cpp` CONTAINS
LINUX-ONLY SYNTAXES FOR LINUX `g++` ⚠️**

For Compiling the **LunarDB CLI** in **Windows**, you'll need to downlad [GCC](https://gcc.gnu.org/install/download.html)
Once done and all setup, you can compile the C++ files with this command:

```bash
g++ -std=c++17 main.cpp cache.cpp saved.cpp sql.cpp module.cpp hashing.cpp -o ../bin/lunar.exe
```

### 🐧Compiling for Linux

To compile **LunarDB** for Linux support, you'll obviously need a **Linux Terminal** for it.
If you are using *Windows*, you'll need to install a **WSL** app (we'll use the Ubuntu WSL for simplicity)

Then run this commands:

```bash
sudo apt update
sudo apt install build-essential
```

After that, head to "src/" and run this to compile for **Linux** support:

```bash
g++ -std=c++17 main.cpp cache.cpp connect.cpp concurrency.cpp saved.cpp sql.cpp module.cpp parser.cpp sharding.cpp hashing.cpp -I/usr/include/lua5.4 -llua5.4 -lpqxx -lpq -lcurl -lboost_system -pthread -o ../bin/lunar
```

## 🤝CONTRIBUTIONS

Feel free to contribution as it is open for all developers! Just make sure to follow the CONTRIBUTION rules and be respectful.

## ©️LICENSE

LunarDB is under the LICENSE of [MIT](https://github.com/Kazooki123/LunarDB/blob/main/LICENSE)

And the [LunarDB License Agreement (LLA)](https://github.com/Kazooki123/LunarDB/blob/main/LUNARDB_LICENSE.txt)

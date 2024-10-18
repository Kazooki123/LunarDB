<div align="center">
  <img src="lunardblogomain.png" alt="LunarDB">

  <h1>🌑LunarDB: A cache key-value database</h1>

  <h3><b>⚡A Cache database⚡</b></h3>
</div>

## 🔗What is it?

Lunar is a Key-Value, Cache, Memory based database that loads keys and values in a file when saved by command, you can load this keys and values by the command "LOAD" then filename.

## 📖Documentation

**💫LunarDB** has a documentation website with a guide for users who are new and learning to use LunarDB!

**🌐Visit** [here](https://lunardbdocs.vercel.app/docs/)

## 🔗Commands

**🧭Available commands:**

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

## 🫂Community

**Join** the LunarDB Community!
- [Discord](https://discord.gg/KgxNzAZ5)

## `lunar.exe` V.S `lunardb.exe`

While it can be confusing at first, they're heavily different:

- lunar.exe - Is the CLI for LunarDB with commands you can execute for your database.
- lunardb.exe - Launches the local **LunarDB Server** using the `lunardb serve` command.

While it may not make sense at first, its my only choice and decision on how
LunarDB functions, everything will changed later or sooner on.

## ⚙️Compiling LunarDB

When it comes to Compiling **LunarDB**, there are few methods to compile it
Currently, We use **G++/GCC** for compiling C++ files and then output them as executables!

### Important Note

**Note:** You would need:

- The Source codes for **Lua v5.4.4**
- For Linux this would be `liblua5.4-dev` (Ubuntu) and `lua-dev` (Arch)

### 🪟Compiling for Windows

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
g++ -o ../bin/lunar main.cpp cache.cpp saved.cpp sql.cpp module.cpp hashing.cpp
```

## 🤝CONTRIBUTIONS

Feel free to contribution as it is open for all developers! Just make sure to follow the CONTRIBUTION rules and be respectful.

## ©️LICENSE

LunarDB is under the LICENSE of MIT.

<div align="center">
  <img src="lunardblogomain.png" alt="LunarDB">

  <h1>🌑LunarDB: A cache key-value database</h1>

  <h3><b>⚡A Blazingly Fast Cache database written in Rust⚡</b></h3>
</div>

## 🔗What is it?

`NOTE`: The maintainer of this project has rewritten the source codes from C++ to Rust.

LunarDB is a **multi-model, key-value store, cache, memory based database** that loads keys and values in a file when saved by command.

## Install

A Guide to **Installing LunarDB**!

### Git clone 💽 (Recommended)

If you want the full source code of **LunarDB** for yourself then you can run:

```bash
git clone https://github.com/Kazooki123/LunarDB.git
```

### Rust 🦀

By using Cargo, simply run:

```bash
cargo add lunardb
```

### 💻 CLI

`NOTE`: If you want to install the `CLI` of LunarDB, you can check the repo in [here](https://github.com/Kazooki123/lunardb-cli)

For Rust using Cargo, you can do:

```bash
cargo install lunardb-cli
```

## 📖 Documentation

**💫LunarDB** has a documentation website with a guide for users who are new and learning to use LunarDB!

**🌐Visit** [here](https://lunardbdocs.vercel.app/docs/)

## 🔗 Commands

**🧭 Basic commands:**

SET key value [ttl] - Set a key-value pair with optional TTL in seconds.

GET key - Get the value for a key.

DEL key - Delete a key-value pair.

MSET key1 value1 key2 value2 ... - Set multiple key-value pairs

MGET key1 key2 ... - Get multiple values

KEYS - List all keys

LUA dofile ('./path/to/your/lua/file.lua')

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

## 🫂 Community

**Join** the LunarDB Community!

- [Discord](https://discord.gg/KgxNzAZ5)

## 🤝 CONTRIBUTIONS

Feel free to contribution as it is open for all developers! Just make sure to follow the [CONTRIBUTION](CONTRIBUTING.md) rules and be respectful.

## ©️ LICENSE

LunarDB is under the LICENSE of [MIT](https://github.com/Kazooki123/LunarDB/blob/main/LICENSE)

And the [LunarDB License Agreement (LLA)](https://github.com/Kazooki123/LunarDB/blob/main/LUNARDB_LICENSE.txt)

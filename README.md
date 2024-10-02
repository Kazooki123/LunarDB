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

CLEAR - Clear all key-value pairs

SIZE - Get the number of key-value pairs

CLEANUP - Remove expired entries

SAVE filename - Save the cache to a file

LOAD filename - Load the cache from a file

QUIT - Exit the program

## `lunar.exe` V.S `lunardb.exe`

While it can be confusing at first, they're heavily different:

- lunar.exe - Is the CLI for LunarDB with commands you can execute for your database.
- lunardb.exe - Launches the local **LunarDB Server** using the `lunardb serve` command.

While it may not make sense at first, its my only choice and decision on how
LunarDB functions, everything will changed later or sooner on.

## 🤝CONTRIBUTIONS

Feel free to contribution as it is open for all developers! Just make sure to follow the CONTRIBUTION rules and be respectful.

## ©️LICENSE

LunarDB is under the LICENSE of MIT.

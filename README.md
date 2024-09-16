# LunarDB: A cache key-value database

A Cache database

## What is it?

Lunar is a Key-Value, Cache, Memory based database that loads keys and values in a file when saved by command, you can load this keys and values by the command "LOAD" then filename.

## Commands

**Available commands:**

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

## CONTRIBUTIONS:
Feel free to contribution as it is open for all developers! Just make sure to follow the CONTRIBUTION rules and be respectful.

## LICENSE

LunarDB is under the LICENSE of MIT.

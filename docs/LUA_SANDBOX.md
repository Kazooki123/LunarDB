# LUA FOR LUNARDB GUIDE

A guide on how to use Lua for custom scripting and sandbox environment testing

## First: What is Lua?

Lua is a **lightweight, high-level, multi-paradigm programming language** designed mainly for
embedded use in applications. Lua is cross-platform software, since the **interpreter** of compiled
bytecode is written in ANSI C.

## What can Lua do for LunarDB?

Lua, primarily known for being a **lightweight, efficient, and embeddable scripting language**, is used in
**LunarDB** for custom scripting and a sandbox environment feature.

With the help of the **Lua API**, LunarDB C++ script can directly execute a Lua file for things like data
management, analysis and benchmarking.

## Executing Lua to the `CLI`

To **execute** a `.lua` file into the LunarDB CLI. You can run this:

```bash
LUA dofile ('./path/to/your/lua/file.lua')
```

But first you'll need a **Lua script to test it**, you can use this as an example:

```lua
-- Set a key-value pair
lunardb_set("lua_key", "Hello from Lua!", 0)

-- Get the value and print it
local value = lunardb_get("lua_key")
print("Value retrieved from LunarDB: " .. value)

-- Perform some calculation
local number = 42
local result = number * 2

-- Store the result in LunarDB
lunardb_set("calculation_result", tostring(result), 60)  -- Store for 60 seconds

print("Calculation result stored in LunarDB with key 'calculation_result'")

```

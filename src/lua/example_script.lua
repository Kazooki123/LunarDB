-- example_script.lua
-- This script demonstrates basic interaction with LunarDB using Lua

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

-- LunarDB Lua Sandbox
-- Provides a secure environment for running Lua scripts with LunarDB

-- Create a restricted environment table
local sandbox_env = {
    -- Basic Lua functions
    assert = assert,
    error = error,
    ipairs = ipairs,
    next = next,
    pairs = pairs,
    pcall = pcall,
    print = print,
    select = select,
    tonumber = tonumber,
    tostring = tostring,
    type = type,
---@diagnostic disable-next-line: deprecated
    unpack = unpack,

    -- Math library (restricted)
    math = {
        abs = math.abs,
        ceil = math.ceil,
        floor = math.floor,
        max = math.max,
        min = math.min,
        random = math.random,
        randomseed = math.randomseed
    },

    -- String library (restricted)
    string = {
        format = string.format,
        len = string.len,
        lower = string.lower,
        upper = string.upper,
        sub = string.sub
    },

    -- Table library (restricted)
    table = {
        insert = table.insert,
        remove = table.remove,
        sort = table.sort
    }
}

-- Add LunarDB specific functions to sandbox
-- These will be implemented in C++ and bound to Lua
sandbox_env.lunardb_get = lunardb_get
sandbox_env.lunardb_set = lunardb_set
sandbox_env.lunardb_del = lunardb_del
sandbox_env.lunardb_exists = lunardb_exists
sandbox_env.lunardb_expire = lunardb_expire
sandbox_env.lunardb_ttl = lunardb_ttl

-- Function to create a new sandboxed environment
function create_sandbox()
    return setmetatable({}, {
        __index = sandbox_env,
        __newindex = function(_, key, _)
            error("Attempt to modify sandbox environment: " .. tostring(key), 2)
        end
    })
end

-- Function to run code in sandbox
function run_in_sandbox(code)
    -- Create new environment
    local env = create_sandbox()

    -- Loads code with custom environment
    local fn, err = load(code, "sandbox", "t", env)
    if not fn then
        return nil, "Failed to load code: " .. err
    end

    -- Run code with pcall for safety
    local success, result = pcall(fn)
    if not success then
        return nil, "Runtime error: " .. result
    end

    return result
end

-- Export sandbox functions
return {
    create_sandbox = create_sandbox,
    run_in_sandbox = run_in_sandbox
}

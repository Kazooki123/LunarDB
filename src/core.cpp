// Authors: Kazooki123, StarloExoliz

/*
** Copyright 2024 Kazooki123
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the “Software”), to deal in the Software without restriction, including without
** limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
** of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
** conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial
** portions of the Software.
**
** THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
** LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
** THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**/

#include "core.h"


std::string decodeSecretValue() {
    std::string encoded = "44 65 61 72 20 4e 69 63 6f 6c 65 0a 0a 45 76 65 72 79 74 69 6d 65 20 " "49 20 73 65 65 2c 20 79 6f 75 72 20 42 65 61 75 74 79 20 73 68 " "69 6e 65 73 20 6d 79 20 6c 69 66 65 2e 0a 0a 57 68 69 6c 65 20 74 68 " "69 73 20 64 61 74 61 62 61 73 65 20 73 74 6f 72 65 73 20 64 61 74 61 " "2c 20 49 27 76 65 20 62 65 65 6e 20 73 74 6f 72 69 6e 67 20 65 76 65 " "72 79 20 6d 6f 6d 65 6e 74 20 77 65 27 76 65 20 73 68 61 72 65 64 20 " "69 6e 20 6d 79 20 6d 65 6d 6f 72 79 2e 0a 0a 53 75 72 65 20 79 6f 75 " "20 6d 69 67 68 74 20 74 68 69 6e 6b 20 74 68 69 73 20 69 73 20 73 75 " "72 70 72 69 73 69 6e 67 2c 20 62 75 74 20 49 20 6a 75 73 74 20 77 61 " "6e 74 20 74 6f 20 73 61 79 20 49 20 68 6f 70 65 20 74 68 69 73 20 6d " "65 73 73 61 67 65 20 67 65 74 73 20 73 65 6e 74 20 61 6e 64 20 62 65 " "65 6e 20 72 65 61 64 20 74 6f 20 74 68 65 20 72 69 67 68 74 20 70 65 " "72 73 6f 6e 2c 20 65 73 70 65 63 69 61 6c 6c 79 20 79 6f 75 2e 0a 0a " "59 6f 75 72 73 20 54 72 75 6c 79 0a 54 61 6b 65 20 61 20 67 75 65 73 " "73 20 3a 29";

    std::string decoded;
    std::istringstream hex_chars_stream(encoded);
    while(hex_chars_stream.good()) {
        std::string hex_char;
        hex_chars_stream >> hex_char;
        char decoded_char = std::stoi(hex_char, nullptr, 16);
        decoded += decoded_char;
    }
    return decoded;
}


// Function implementations
void printColoredText(const std::string& text, const std::string& color) {
    if (color == "blue") {
        std::cout << "\033[34m" << text << "\033[0m";
    } else if (color == "yellow") {
        std::cout << "\033[33m" << text << "\033[0m";
    } else if (color == "red") {
        std::cout << "\033[31m" << text << "\033[0m";
    } else {
        std::cout << text;
    }
}

void printHelp() {
    std::cout << "Available commands:\n"
              << "SET key value [ttl] - Set a key-value pair with optional TTL in seconds\n"
              << "GET key - Get the value for a key\n"
              << "DEL key - Delete a key-value pair\n"
              << "MSET key1 value1 key2 value2 ... - Set multiple key-value pairs\n"
              << "MGET key1 key2 ... - Get multiple values\n"
              << "KEYS - List all keys\n"
              << "SWITCH - Switches to SCHEMAFULL, SCHEMALESS or SQL\n"
              << "CONNECT - Connects to a local LunarDB server in your machine (which is launched)\n"
              << "RUNLRQL - Runs a '.lrql' query file, a file extension for LunarDB Extended Multi-model feature\n"
              << "CLEAR - Clear all key-value pairs\n"
              << "MODULE ADD module_name - Adds a module to your LunarDB modules if needed\n"
              << "MODULE LIST - Lists all modules you have downloaded\n"
              << "PROVIDER HELP - Lists commands for the Provider subcommands\n"
              << "HASH SHA256 key - Hashes a key using SHA-256\n"
              << "HASH MURMUR3 key - Hashes a key using the MURMUR3 encryption method\n"
              << "HASH ROTATE input shift - Rotates a hash base on the amount of shifts\n"
              << "SIZE - Get the number of key-value pairs\n"
              << "CLEANUP - Remove expired entries\n"
              << "THREADS - Shows Active threads and Queued tasks\n"
              << "SAVE filename - Save the cache to a file\n"
              << "LOAD filename - Load the cache from a file\n"
              << "LUA dofile ('./path/to/lua/file.lua') - Executes a lua file as a Sandbox\n"
              << "LPUSH key value - Push an element to the head of the list\n"
              << "LPOP key - Remove and return an element from the head of the list\n"
              << "RPUSH key value - Push an element to the tail of the list\n"
              << "RPOP key - Remove and return an element from the tail of the list\n"
              << "LRANGE key start stop - Get a range of elements from the list\n"
              << "LLEN key - Get the length of the list\n"
              << "QUIT - Exit the program\n";
}

void printLunarLogo() {
    std::cout << "\033[35m";
    std::cout << "___           ___  ___      ________       ________      ________      \n"
              << "|\\  \\         |\\  \\|\\  \\    |\\   ___  \\    |\\   __  \\    |\\   __  \\    \n"
              << "\\ \\  \\        \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\  \\|\\  \\   \\ \\  \\|\\  \\   \n"
              << " \\ \\  \\        \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\   __  \\   \\ \\   _  _\\  \n"
              << "  \\ \\  \\____    \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\  \\ \\  \\   \\ \\  \\\\  \\| \n"
              << "   \\ \\_______\\   \\ \\_______\\   \\ \\__\\\\ \\__\\   \\ \\__\\ \\__\\   \\ \\__\\\\ _\\ \n"
              << "    \\|_______|    \\|_______|    \\|__| \\|__|    \\|__|\\|__|    \\|__|\\|__|\n";
    std::cout << "\033[0m";
}

void printSwitchOptions() {
    std::cout << "Pick a type:\n";
    printColoredText("> SCHEMAFULL\n", "blue");
    printColoredText("> SCHEMALESS\n", "yellow");
    printColoredText("> SQL\n", "red");
}

void printProviderHelp() {
    std::cout << "Provider commands:\n"
              << "  PROVIDER LIST                - List all available database providers\n"
              << "  PROVIDER ATTACH <name>       - Connect to a database provider\n"
              << "  PROVIDER DETACH             - Disconnect current provider\n"
              << "  PROVIDER STATUS             - Show current provider status\n";
}

bool isSecretCommand(const std::string& input) {
    const std::string secret = "--nicole";
    std::hash<std::string> hasher;
    return hasher(input) == hasher(secret);
}

bool getProviderConfig(lunardb::providers::ProviderConfig& config) {
    std::cout << "\nDatabase Connection Setup\n";
    std::cout << "------------------------\n";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Host [localhost]: ";
    std::getline(std::cin, config.host);
    if (config.host.empty()) config.host = "localhost";

    std::cout << "Port [5432]: ";
    std::string portStr;
    std::getline(std::cin, portStr);
    config.port = portStr.empty() ? 5432 : std::stoi(portStr);

    std::cout << "Database name: ";
    std::getline(std::cin, config.database);
    if (config.database.empty()) {
        std::cout << "Database name cannot be empty\n";
        return false;
    }

    std::cout << "Username: ";
    std::getline(std::cin, config.username);
    if (config.username.empty()) {
        std::cout << "Username cannot be empty\n";
        return false;
    }

    std::cout << "Password: ";
    config.password = "";
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        config.password += ch;
        std::cout << '*';
    }
    std::cout << std::endl;

    return true;
}

/* Lua Integration */
/*
** But Why?
** Since Lua is good for external scripting and custom
** commands making, I have decided to use Lua
** for LunarDB integration
**/

/*==========================================================================*/

// Wrapper function to handle C++ exceptions
template<typename Func>
static int lua_cppfunction(lua_State* L, Func f) {
    try {
        return f(L);
    } catch (const std::exception& e) {
        lua_pushstring(L, e.what());
        return lua_error(L);
    } catch (...) {
        lua_pushstring(L, "Unknown C++ exception");
        return lua_error(L);
    }
}

int lua_set(lua_State* L) {
    return lua_cppfunction(L, [](lua_State* L) {
        const char* key = luaL_checkstring(L, 1);
        const char* value = luaL_checkstring(L, 2);
        int ttl = luaL_optinteger(L, 3, 0);
        cache.set(key, value, ttl);
        return 0;
    });
}

int lua_get(lua_State* L) {
    return lua_cppfunction(L, [](lua_State* L) {
        const char* key = luaL_checkstring(L, 1);
        std::string value = cache.get(key);
        lua_pushstring(L, value.c_str());
        return 1;
    });
}

void initializeLua() {
    L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, lua_set);
    lua_setglobal(L, "lunardb_set");

    lua_pushcfunction(L, lua_get);
    lua_setglobal(L, "lunardb_get");
}

void closeLua() {
    if (L) {
        lua_close(L);
        L = nullptr;
    }
}

bool executeLuaScript(const std::string& script) {
    if (luaL_dostring(L, script.c_str()) != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);  // Remove error message from stack
        return false;
    }
    return true;
}

/*==================================END=====================================*/

bool checkHealth() {
    try {
        cache.set("health_check", "test");
        std::string result = cache.get("health_check");
        if (result != "test") {
            return false;
        }
        cache.del("health_check");

        if (!L || luaL_dostring(L, "return true") != LUA_OK) {
            return false;
        }

        return true;
    } catch (...) {
        return false;
    }
}

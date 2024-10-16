// Author: Kazooki123, StarloExoliz, Smartlinuxcoder

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


#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iomanip>

extern "C" {
    #include <lua5.4/lua.h>
    #include <lua5.4/lauxlib.h>
    #include <lua5.4/lualib.h>
}


#include "cache.h"
#include "saved.h"
#include "sql.h"
#include "module.h"
#include "sharding.h"
#include "hashing.h"
#include "connect.h"


enum class Mode {
    SCHEMAFULL,
    SCHEMALESS,
    SQL
};

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

template<typename F, typename... Args>
double measureExecutionTime(F func, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

void printSwitchOptions() {
    std::cout << "Pick a type:\n";
    printColoredText("> SCHEMAFULL\n", "blue");
    printColoredText("> SCHEMALESS\n", "yellow");
    printColoredText("> SQL\n", "red");
}

void printHelp() {
    std::cout << "Available commands:\n"
              << "SET key value [ttl] - Set a key-value pair with optional TTL in seconds\n"
              << "GET key - Get the value for a key\n"
              << "DEL key - Delete a key-value pair\n"
              << "MSET key1 value1 key2 value2 ... - Set multiple key-value pairs\n"
              << "MGET key1 key2 ... - Get multiple values\n"
              << "KEYS - List all keys\n"
              << "SWITCH - Switches to SCHEMAFULL, SCHEMALESS or SQL (Do not attempt to command this as it's broken\n"
              << "CONNECT - Connects to a local LunarDB server in your machine (which is launched)\n"
              << "CLEAR - Clear all key-value pairs\n"
              << "MODULE ADD module_name - Adds a module to your LunarDB modules if needed\n"
              << "MODULE LIST - Lists all modules you have downloaded\n"
              << "HASH SHA256 key - Hashes a key using SHA-256\n"
              << "HASH MURMUR3 key - Hashes a key using the MURMUR3 encryption method\n"
              << "HASH ROTATE input shift - Rotates a hash base on the amount of shifts\n"
              << "SIZE - Get the number of key-value pairs\n"
              << "CLEANUP - Remove expired entries\n"
              << "SAVE filename - Save the cache to a file\n"
              << "LOAD filename - Load the cache from a file\n"
              << "LUA dofile <file_to_your_lua_script> \n"
              << "LPUSH key value - Push an element to the head of the list\n"
              << "LPOP key - Remove and return an element from the head of the list\n"
              << "RPUSH key value - Push an element to the tail of the list\n"
              << "RPOP key - Remove and return an element from the tail of the list\n"
              << "LRANGE key start stop - Get a range of elements from the list\n"
              << "LLEN key - Get the length of the list\n"
              << "QUIT - Exit the program\n";
}

// 3D ASCII TEXT ART FOR LUNARDB
void printLunarLogo() {
    std::cout << "\033[35m"; // Sets the text color to purple
    std::cout << "___           ___  ___      ________       ________      ________      \n"
              << "|\\  \\         |\\  \\|\\  \\    |\\   ___  \\    |\\   __  \\    |\\   __  \\    \n"
              << "\\ \\  \\        \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\  \\|\\  \\   \\ \\  \\|\\  \\   \n"
              << " \\ \\  \\        \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\   __  \\   \\ \\   _  _\\  \n"
              << "  \\ \\  \\____    \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\  \\ \\  \\   \\ \\  \\\\  \\| \n"
              << "   \\ \\_______\\   \\ \\_______\\   \\ \\__\\\\ \\__\\   \\ \\__\\ \\__\\   \\ \\__\\\\ _\\ \n"
              << "    \\|_______|    \\|_______|    \\|__| \\|__|    \\|__|\\|__|    \\|__|\\|__|\n";
    std::cout << "\033[0m"; // Resets the text color to default
}

/* Lua Integration */
/*
** But Why?
** Since Lua is good for external scripting and custom
** commands making, I have decided to use Lua
** for LunarDB integration
**/

/*==========================================================================*/

Cache cache(1000);
lua_State* L = nullptr;

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

int main() {
    // Removed Cache local variable as it is moved to be global
    SQL sql(cache);
    ModuleManager moduleManager;
    Hashing hashing;
    std::string command, line;
    Mode currentMode = Mode::SCHEMALESS;

    printLunarLogo();
    std::cout << "Welcome to LunarDB! A Redis-like cache database!\n";
    printHelp();

    initializeLua();

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        std::istringstream iss(line);
        iss >> command;

        if (currentMode == Mode::SQL) {
            std::string result = sql.executeQuery(line);
            std::cout << result << "\n";
        } else {
            if (command == "SET") {
                std::string key, value;
                int ttl = 0;
                if (iss >> key >> value) {
                    iss >> ttl;
                    double executionTime = measureExecutionTime([&]() {
                        cache.set(key, value, ttl);
                    });
                    std::cout << "OK (" << std::fixed << std::setprecision(3) << executionTime << "ms)\n";
                } else {
                    std::cout << "Invalid SET command\n";
                }
            } else if (command == "GET") {
                std::string key;
                if (iss >> key) {
                    std::string result;
                    double executionTime = measureExecutionTime([&]() {
                        result = cache.get(key);
                    });
                    if (!result.empty()) {
                        std::cout << result << " (" << std::fixed << std::setprecision(3) << executionTime << "ms)\n";
                    } else {
                        std::cout << "(nil) (" << std::fixed << std::setprecision(3) << executionTime << "ms)\n";
                    }
                } else {
                    std::cout << "Invalid GET command\n";
                }
            } else if (command == "ping") {
                std::cout << "PONG!\n";
            } else if (command == "SWITCH") {
                printSwitchOptions();
                std::string modeStr;
                std::cin >> modeStr;
                std::transform(modeStr.begin(), modeStr.end(), modeStr.begin(), ::toupper);

                if (modeStr == "SCHEMAFULL") {
                    currentMode = Mode::SCHEMAFULL;
                    std::cout << "Switched to SCHEMAFULL mode\n";
                } else if (modeStr == "SCHEMALESS") {
                    currentMode = Mode::SCHEMALESS;
                    std::cout << "Switched to SCHEMALESS mode\n";
                } else if (modeStr == "SQL") {
                    currentMode = Mode::SQL;
                    std::cout << "Switched to SQL mode\n";
                } else {
                    std::cout << "Invalid mode. Please choose SCHEMAFULL, SCHEMALESS, or SQL\n";
                }
            } else if (command == "LUA") {
                std::string script;
                std::getline(iss, script);
                if (executeLuaScript(script)) {
                    std::cout << "Lua script executed successfully.\n";
                }
            } else if (command == "MODULE") {
                std::string subCommand;
                iss >> subCommand;
            
                if (subCommand == "ADD") {
                    std::string moduleName;
                    if (iss >> moduleName) {
                        if (moduleManager.addModule(moduleName)) {
                            std::cout << "Module " << moduleName << " added successfully.\n";
                        } else {
                            std::cout << "Module " << moduleName << " already exists.\n";
                        }
                    } else {
                        std::cout << "Invalid MODULE ADD command. Usage: MODULE ADD <module_name>\n";
                    }
                } else if (subCommand == "LIST") {
                    auto modules = moduleManager.listModules();
                    if (modules.empty()) {
                        std::cout << "No modules installed.\n";
                    } else {
                        std::cout << "Installed modules:\n";
                        for (const auto& module : modules) {
                            std::cout << "- " << module << "\n";
                        }
                    }
                } else if (subCommand == "REMOVE") {
                    std::string moduleName;
                    if (iss >> moduleName) {
                        if (moduleManager.removeModule(moduleName)) {
                            std::cout << "Module " << moduleName << " removed successfully.\n";
                        } else {
                            std::cout << "Failed to remove module " << moduleName << ". It may not exist.\n";
                        }
                    } else {
                        std::cout << "Invalid MODULE REMOVE command. Usage: MODULE REMOVE <module_name>\n";
                    } 
                } else {
                    std::cout << "Unknown MODULE subcommand. Available subcommands: ADD, LIST, REMOVE\n";
                }
            } else if (command == "HASH" ) {
                std::string subCommand, input;
                int shift = 0;
                if (iss >> subCommand >> input) {
                    if (subCommand == "SHA256") {
                        std::cout << Hashing::sha256(input) << "\n";
                    } else if (subCommand == "MURMUR3") {
                        std::cout << Hashing::murmur3_32(input) << "\n";
                    } else if (subCommand == "ROTATE") {
                        if (iss >> shift) {
                            std::cout << Hashing::rotateEncrypt(input, shift) << "\n";
                        } else {
                            std::cout << "Invalid HASH ROTATE command. Usage: HASH ROTATE <input> <shift>\n";
                        }
                    } else {
                        std::cout << "Invalid HASH subcommand. Available subcommands: SHA256, MURMUR3, ROTATE\n";
                    }
                } else {
                    std::cout << "Invalid HASH command. Usage: HASH <subcommand> <input> [shift]\n";
                }
            } else if (command == "DEL") {
                std::string key;
                if (iss >> key) {
                    if (cache.del(key)) {
                        std::cout << "OK\n";
                    } else {
                        std::cout << "(nil)\n";
                    }
                } else {
                    std::cout << "Invalid DEL command\n";
                }
            } else if (command == "MSET") {
                std::vector<std::pair<std::string, std::string>> kvs;
                std::string key, value;
                while (iss >> key >> value) {
                    kvs.emplace_back(key, value);
                }
                if (!kvs.empty()) {
                    cache.mset(kvs);
                    std::cout << "OK\n";
                } else {
                    std::cout << "Invalid MSET command\n";
                }
            } else if (command == "MGET") {
                std::vector<std::string> keys;
                std::string key;
                while (iss >> key) {
                    keys.push_back(key);
                }
                if (!keys.empty()) {
                    auto results = cache.mget(keys);
                    for (const auto& result : results) {
                        if (!result.empty()) {
                            std::cout << result << "\n";
                        } else {
                            std::cout << "(nil)\n";
                        }
                    }
                } else {
                    std::cout << "Invalid MGET command\n";
                }
            } else if (command == "KEYS") {
                auto keys = cache.keys();
                for (const auto& key : keys) {
                    std::cout << key << "\n";
                }
                if (keys.empty()) {
                    std::cout << "(empty list)\n";
                }
            } else if (command == "CLEAR") {
                cache.clear();
                std::cout << "OK\n";
            } else if (command == "SIZE") {
                std::cout << cache.size() << "\n";
            } else if (command == "CLEANUP") {
                cache.cleanup_expired();
                std::cout << "OK\n";
            } else if (command == "SAVE") {
                std::string filename;
                if (iss >> filename) {
                    Saved::saveToFile(cache, filename);
                    std::cout << "OK\n";
                } else {
                    std::cout << "Invalid SAVE command. Usage: SAVE filename\n";
                }
            } else if (command == "LOAD") {
                std::string filename;
                if (iss >> filename) {
                    Saved::loadFromFile(cache, filename);
                    std::cout << "OK\n";
                } else {
                    std::cout << "Invalid LOAD command. Usage: LOAD filename\n";
                }
            } else if (command == "LPUSH") {
                std::string key, value;
                if (iss >> key >> value) {
                    cache.lpush(key, value);
                    std::cout << "OK\n";
                } else {
                    std::cout << "Invalid LPUSH command\n";
                }
            } else if (command == "LPOP") {
                std::string key;
                if (iss >> key) {
                    std::string value = cache.lpop(key);
                    if (!value.empty()) {
                        std::cout << value << "\n";
                    } else {
                        std::cout << "(nil)\n";
                    }
                } else {
                    std::cout << "Invalid LPOP command\n";
                }
            } else if (command == "RPUSH") {
                std::string key, value;
                if (iss >> key >> value) {
                    cache.rpush(key, value);
                    std::cout << "OK\n";
                } else {
                    std::cout << "Invalid RPUSH command\n";
                }
            } else if (command == "RPOP") {
                std::string key;
                if (iss >> key) {
                    std::string value = cache.rpop(key);
                    if (!value.empty()) {
                        std::cout << value << "\n";
                    } else {
                        std::cout << "(nil)\n";
                    }
                } else {
                    std::cout << "Invalid RPOP command\n";
                }
            } else if (command == "LRANGE") {
                std::string key;
                int start, stop;
                if (iss >> key >> start >> stop) {
                    auto values = cache.lrange(key, start, stop);
                    for (const auto& value : values) {
                        std::cout << value << "\n";
                    }
                    if (values.empty()) {
                        std::cout << "(empty list or set)\n";
                    }
                } else {
                    std::cout << "Invalid LRANGE command\n";
                }
            } else if (command == "LLEN") {
                std::string key;
                if (iss >> key) {
                    std::cout << cache.llen(key) << "\n";
                } else {
                    std::cout << "Invalid LLEN command\n";
                }
            } else if (command == "CONNECT") {
                std::string IPandPort;
                if (iss >> IPandPort) {
                    
                }
            } else if (command == "QUIT") {
                break;
            } else {
                std::cout << "Unknown command. Type 'HELP' for available commands.\n";
            }
        }
    }

    closeLua();
    std::cout << "Goodbye!\n";
    return 0;
}

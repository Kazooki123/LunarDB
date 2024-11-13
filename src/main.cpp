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

// To compile we use and run ~> "g++ -std=c++17 main.cpp cache.cpp saved.cpp sql.cpp module.cpp hashing.cpp -I/usr/include/lua5.4 -llua5.4 -lpqxx -lpq -o ../bin/lunar"

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

// extern "C" {
//    #include "lua/src/lua.h"
//    #include "lua/src/lauxlib.h"
//    #include "lua/src/lualib.h"
// }

#include "cache.h"
#include "saved.h"
#include "sql.h"
#include "module.h"
#include "sharding.h"
#include "hashing.h"
#include "connect.h"

#include "providers/provider.hpp"
#include "providers/registry.hpp"
#include "providers/postgresql_provider.hpp"

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

bool isSecretCommand(const std::string& input) {
    const std::string secret = "--nicole";
    std::hash<std::string> hasher;
    
    auto input_hash = hasher(input);
    auto secret_hash = hasher(secret);

    std::cout << "Debug: Input hash = " << std::hex << input_hash << std::endl;
    std:: cout << "Debug: Expected hash = " << std::hex << secret_hash << std::endl;

    return input_hash == secret_hash;
}

void printProviderHelp() {
    std::cout << "Provider commands:\n"
              << "  PROVIDER LIST                - List all available database providers\n"
              << "  PROVIDER ATTACH <name>       - Connect to a database provider\n"
              << "  PROVIDER DETACH             - Disconnect current provider\n"
              << "  PROVIDER STATUS             - Show current provider status\n";
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
              << "CLEAR - Clear all key-value pairs\n"
              << "MODULE ADD module_name - Adds a module to your LunarDB modules if needed\n"
              << "MODULE LIST - Lists all modules you have downloaded\n"
              << "PROVIDER HELP - Lists commands for the Provider subcommands\n"
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

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::cout << "Debug: Argument received:" << argv[1] << std::endl;
    }
    if (argc > 1 && isSecretCommand(argv[1])) {
        std::cout << decodeSecretValue() << std::endl;
        return 0;
    }

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
            } else if (command == "PROVIDER") {
                std::string subCommand;
                iss >> subCommand;
    
                if (subCommand == "HELP") {
                    printProviderHelp();
                }
                else if (subCommand == "LIST") {
                    auto providers = lunardb::providers::ProviderRegistry::instance().getAvailableProviders();
                    std::cout << "\nAvailable database providers:\n";
                    std::cout << "----------------------------\n";
                    for (const auto& provider : providers) {
                        std::cout << "- " << provider << "\n";
                    }
                }
                else if (subCommand == "ATTACH") {
                    std::string providerName;
                    if (iss >> providerName) {
                        auto provider = lunardb::providers::ProviderRegistry::instance().createProvider(providerName);
                        if (provider) {
                            lunardb::providers::ProviderConfig config;
                
                            if (getProviderConfig(config)) {
                                std::cout << "\nConnecting to database...\n";
                    
                                try {
                                    if (provider->connect(config)) {
                                        if (cache.attachProvider(std::move(provider))) {
                                            std::cout << "✓ Successfully connected to " << providerName << " database\n";
                                        } else {
                                            std::cout << "✗ Failed to attach provider to cache\n";
                                        }
                                    } else {
                                        std::cout << "✗ Failed to connect to database\n";
                                    }
                                } catch (const std::exception& e) {
                                    std::cout << "✗ Error connecting to database: " << e.what() << "\n";
                                }
                            }
                        } else {
                            std::cout << "✗ Unknown provider: " << providerName << "\n";
                            std::cout << "Use 'PROVIDER LIST' to see available providers\n";
                        }
                    } else {
                        std::cout << "Usage: PROVIDER ATTACH <provider_name>\n";
                        std::cout << "Example: PROVIDER ATTACH postgresql\n";
                    }
                }
                else if (subCommand == "DETACH") {
                    if (cache.hasProvider()) {
                        cache.detachProvider();
                        std::cout << "✓ Provider detached successfully\n";
                    } else {
                        std::cout << "! No provider currently attached\n";
                    }
                }
                else if (subCommand == "STATUS") {
                    if (cache.hasProvider()) {
                        std::cout << "Provider status: Connected\n";
                    } else {
                        std::cout << "Provider status: No provider attached\n";
                    }
                }
                else {
                    std::cout << "Unknown PROVIDER subcommand. Use 'PROVIDER HELP' for available commands.\n";
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
                std::cout << "Unknown command. Check for LunarDB commands ABOVE.\n";
            }
        }
    }

    closeLua();
    std::cout << "Goodbye! :) Have a lovely day!\n";
    return 0;
}

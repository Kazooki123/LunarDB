// Author: Kazooki123, StarloExoliz

// Copyright 2024 Kazooki123

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include <iostream>
#include <string>
#include <sstream>
#include "cache.h"
#include "saved.h"
#include "sql.h"
#include "module.h"
#include "sharding.h"
#include <algorithm>

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
              << "CONNECT - Connects to a local LunarDB server in your machine(which is launched)\n"
              << "CLEAR - Clear all key-value pairs\n"
              << "MODULE ADD - Adds a module to your LunarDB modules if needed\n"
              << "HASH - Hashes a key or value using SHA-256\n"
              << "MODULE LIST - Lists all modules you have downloaded\n"
              << "SIZE - Get the number of key-value pairs\n"
              << "CLEANUP - Remove expired entries\n"
              << "SAVE filename - Save the cache to a file\n"
              << "LOAD filename - Load the cache from a file\n"
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
    std::cout << "___           ___  ___      ________       ________      ________      \n"
              << "|\\  \\         |\\  \\|\\  \\    |\\   ___  \\    |\\   __  \\    |\\   __  \\    \n"
              << "\\ \\  \\        \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\  \\|\\  \\   \\ \\  \\|\\  \\   \n"
              << " \\ \\  \\        \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\   __  \\   \\ \\   _  _\\  \n"
              << "  \\ \\  \\____    \\ \\  \\\\\\  \\   \\ \\  \\\\ \\  \\   \\ \\  \\ \\  \\   \\ \\  \\\\  \\| \n"
              << "   \\ \\_______\\   \\ \\_______\\   \\ \\__\\\\ \\__\\   \\ \\__\\ \\__\\   \\ \\__\\\\ _\\ \n"
              << "    \\|_______|    \\|_______|    \\|__| \\|__|    \\|__|\\|__|    \\|__|\\|__|\n";

}

int main() {
    Cache cache(1000); // Create a cache with a maximum of 1000 entries
    SQL sql(cache);
    ModuleManager moduleManager;
    std::string command, line;
    Mode currentMode = Mode::SCHEMALESS;

    printLunarLogo();
    std::cout << "Welcome to Lunar! A Red cache database!\n";
    printHelp();

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        std::istringstream iss(line);
        iss >> command;

        if (command == "SWITCH") {
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
            continue;
        }

        if (currentMode == Mode::SQL) {
            std::string result = sql.executeQuery(line);
            std::cout << result << "\n";
        } else {
            if (command == "SET") {
                std::string key, value;
                int ttl = 0;
                if (iss >> key >> value) {
                    iss >> ttl;
                    cache.set(key, value, ttl);
                    std::cout << "OK\n";
                } else {
                    std::cout << "Invalid SET command\n";
                }
            } else if (command == "GET") {
                std::string key;
                if (iss >> key) {
                    std::string result = cache.get(key);
                    if (!result.empty()) {
                        std::cout << result << "\n";
                    } else {
                        std::cout << "(nil)\n";
                    }
                } else {
                    std::cout << "Invalid GET command\n";
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
            } else if (command == "QUIT") {
                break;
            } else {
                std::cout << "Unknown command. Type 'HELP' for available commands.\n";
            }
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}

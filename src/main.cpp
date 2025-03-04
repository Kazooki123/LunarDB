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

// To compile we use and run ~> "g++ -std=c++17 main.cpp core_MAIN.cpp cache.cpp saved.cpp sql.cpp parser.cpp sharding.cpp hashing.cpp -I/usr/include/lua5.4 -llua5.4 -lpq -lboost_system -pthread -o ../bin/lunardb"

// For Linux users that don't use Ubuntu and uses other Linux distros, please refer to the COMPILEGUIDE markdown file guide.

#include "core_MAIN.h"

// Global Variables
Cache cache(1000);
LunarDB::Connection connection;
TaskQueue taskQueue;
BackgroundProcessor bgProcessor;
LunarDB::ShardManager shardManager(4);
lua_State* L = nullptr;

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::cout << "Debug: Argument received:" << argv[1] << std::endl;

        if (strcmp(argv[1], "--health") == 0) {
            bool isHealthy = checkHealth();
            if (isHealthy) {
                std::cout << "healthy" << std::endl;
                return 0;
            } else {
                std::cout << "unhealthy" << std::endl;
                return 1;
            }
        }
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

    taskQueue.startBackgroundTasks();
    bgProcessor.start();

    // Schedules background tasks
    bgProcessor.scheduleCleanup();  // Auto cleanup every 5 minutes
    bgProcessor.schedulePersistence("autosave.db");  // Auto save every 15 minutes

    initializeLua();

    // int apiPort = 8080;  // default port
    // for (int i = 1; i < argc; i++) {
    //     if (std::string(argv[i]) == "--api-port" && i + 1 < argc) {
    //         apiPort = std::stoi(argv[i + 1]);
    //         i++;
    //     }
    // }

    // std::thread apiThread(startApiServer, apiPort);
    // std::cout << "API server started on port " << apiPort << std::endl;

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
            } else if (command == "SHARD") {
                std::string subCommand;
                if (iss >> subCommand) {
                    if (subCommand == "INFO") {
                        std::cout << "Total Shards: " << shardManager.getShardCount() << "\n";
                        std::cout << "Total Keys: " << shardManager.getTotalKeyCount() << "\n";
                    } else if (subCommand == "REBALANCE") {
                        int newShardCount;
                        if (iss >> newShardCount && newShardCount > 0) {
                            try {
                                shardManager.rebalance(newShardCount);
                                std::cout << "Shards rebalanced to " << newShardCount << " shards\n";
                            } catch (const std::exception& e) {
                                std::cout << "Rebalancing failed: " << e.what() << "\n";
                            }
                        } else {
                            std::cout << "Invalid shard count\n";
                        }
                    } else if (subCommand == "LOCATE") {
                        std::string key;
                        if (iss >> key) {
                            size_t shardIndex = shardManager.getShardIndex(key);
                            std::cout << "Key '" << key << "' is located in Shard " << shardIndex << "\n";
                        } else {
                            std::cout << "Please provide a key\n";
                        }
                    } else {
                        std::cout << "Shard subcommands:\n"
                                << "  SHARD INFO         - Show number of shards\n"
                                << "  SHARD CREATE <n>   - Create n shards\n"
                                << "  SHARD LOCATE <key> - Find which shard a key belongs to\n";
                    }
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
                    std::string repoUrl;

                    if (iss >> moduleName) {
                        if (moduleManager.installModule(moduleName, repoUrl)) {
                            std::cout << "Module " << moduleName << " added and installed successfully.\n";
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
            } else if (command == "THREADS") {
                std::cout << "Active threads: " << taskQueue.getActiveThreadCount() << "\n";
                std::cout << "Queued tasks: " << taskQueue.getQueueSize() << "\n";
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
            } else if (command == "RUNLRQL") {
                std::string filename;
                if (iss >> filename) {
                    if (filename.substr(filename.find_last_of(".") + 1) == "lrql") {
                        std::ifstream file(filename);
                        if (file) {
                            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                            // Check if file contains TABLE keyword
                            if (content.find("TABLE") != std::string::npos) {
                                try {
                                    Tokenizer tokenizer(content);
                                    auto tokens = tokenizer.tokenize();
                                    Parser parser(tokens);
                                    parser.parse();
                                    std::cout << "Success: LRQL file executed successfully\n";
                                } catch (const std::exception& e) {
                                    std::cout << "Error: Failed to execute LRQL file - " << e.what() << "\n";
                                }
                            } else {
                                std::cout << "Error: No TABLE definition found in the LRQL file\n";
                            }
                        } else {
                            std::cout << "Error: Failed to open file: " << filename << "\n";
                        }
                    } else {
                        std::cout << "Error: Invalid file type. Only .lrql files are supported.\n";
                    }
                } else {
                    std::cout << "Usage: RUNLRQL file.lrql\n";
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
                std::string host = "127.0.0.1";
                int port = 6379;
                std::string arg;
                while (iss >> arg) {
                    if (arg == "--host" || arg == "-h") {
                        if (!(iss >> host)) {
                            std::cout << "Invalid CONNECT command: Missing host value\n";
                            return 1;
                        }
                    } else if (arg == "--port" || arg == "-p") {
                        if (!(iss >> port)) {
                            std::cout << "Invalid CONNECT command: Missing port value\n";
                            return 1;
                        }
                        if (port <= 0 || port > 65535) {
                            std::cout << "Invalid CONNECT command: Port must be between 1-65535\n";
                            return 1;
                        }
                    } else {
                        std::cout << "Invalid CONNECT argument: " << arg << "\n";
                        return 1;
                    }
                }

                try {
                    connection.connect(host, port);
                    std::cout << "Connected to " << host << ":" << port << "\n";
                } catch (const std::exception& e) {
                    std::cout << "Failed to connect to " << host << ":" << port << "\n";
                    return 1;
                }
            } else if (command == "QUIT") {
                break;
            } else {
                std::cout << "Unknown command. Check for LunarDB commands ABOVE.\n";
            }
        }
    }

    bgProcessor.stop();
    taskQueue.stopBackgroundTasks();

    closeLua();
    std::cout << "Goodbye! :) Have a lovely day!\n";
    return 0;
}

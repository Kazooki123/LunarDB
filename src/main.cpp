#include <iostream>
#include <string>
#include <sstream>
#include "cache.h"
#include "saved.h"

void printHelp() {
    std::cout << "Available commands:\n"
              << "SET key value [ttl] - Set a key-value pair with optional TTL in seconds\n"
              << "GET key - Get the value for a key\n"
              << "DEL key - Delete a key-value pair\n"
              << "MSET key1 value1 key2 value2 ... - Set multiple key-value pairs\n"
              << "MGET key1 key2 ... - Get multiple values\n"
              << "KEYS - List all keys\n"
              << "CLEAR - Clear all key-value pairs\n"
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

int main() {
    Cache cache(1000); // Create a cache with a maximum of 1000 entries
    std::string command, line;

    std::cout << "Welcome to Lunar! A Redis-like cache database!\n";
    printHelp();

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        std::istringstream iss(line);
        iss >> command;

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

    std::cout << "Goodbye!\n";
    return 0;
}

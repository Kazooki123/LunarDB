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




#include "saved.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>
#include <sstream>
#include <filesystem>

std::string generateRandomHash() {
    const char* hex_chars = "0123456789abcdef";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << hex_chars[dis(gen)];
    }
    return ss.str();
}

void Saved::saveToFile(const Cache& cache, const std::string& directory) {
    std::filesystem::create_directories(directory);
    std::string filename = directory + "/" + generateRandomHash() + ".cache";
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
        return;
    }

    // Writes a LUNAR header for verification so that it prevents malicious backdoor or anonymous loads of data
    file << "LUNAR_CACHE_V1\n";

    auto keys = cache.keys();
    for (const auto& key : keys) {
        std::string value = cache.get(key);
        file << key.length() << "\n" << key << "\n";
        file << value.length() << "\n" << value << "\n";
    }

    file.close();
    std::cout << "Cache saved to file: " << filename << std::endl;
}

void Saved::loadFromFile(Cache& cache, const std::string& directory) {
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.path().extension() == ".cache") {
            std::ifstream file(entry.path(), std::ios::binary);
            if (!file) {
                std::cerr << "Error: Unable to open file for reading: " << entry.path() << std::endl;
                continue;
            }

            std::string header;
            std::getline(file, header);
            if (header != "LUNAR_CACHE_V1") {
                std::cerr << "Error: Invalid cache file format: " << entry.path() << std::endl;
                continue;
            }

            std::string lenStr, key, value;
            while (std::getline(file, lenStr)) {
                int keyLen = std::stoi(lenStr);
                key.resize(keyLen);
                file.read(&key[0], keyLen);
                file.ignore(); // Skip newline

                std::getline(file, lenStr);
                int valueLen = std::stoi(lenStr);
                value.resize(valueLen);
                file.read(&value[0], valueLen);
                file.ignore(); // Skip newline

                cache.set(key, value);
            }

            file.close();
            std::cout << "Cache loaded from file: " << entry.path() << std::endl;
            return; // Load only the first valid cache file found
        }
    }
    std::cerr << "Error: No valid cache file found in directory: " << directory << std::endl;
}

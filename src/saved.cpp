#include "saved.h"
#include <fstream>
#include <iostream>
#include <chrono>

void Saved::saveToFile(const Cache& cache, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
        return;
    }

    auto keys = cache.keys();
    for (const auto& key : keys) {
        std::string value = cache.get(key);
        file << key << "\n" << value << "\n";
    }

    file.close();
    std::cout << "Cache saved to file: " << filename << std::endl;
}

void Saved::loadFromFile(Cache& cache, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file for reading: " << filename << std::endl;
        return;
    }

    std::string key, value;
    while (std::getline(file, key) && std::getline(file, value)) {
        cache.set(key, value);
    }

    file.close();
    std::cout << "Cache loaded from file: " << filename << std::endl;
}
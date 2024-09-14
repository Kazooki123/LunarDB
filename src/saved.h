#ifndef SAVED_H
#define SAVED_H

#include "cache.h"
#include <string>

class Saved {
public:
    static void saveToFile(const Cache& cache, const std::string& filename);
    static void loadFromFile(Cache& cache, const std::string& filename);
};

#endif // SAVED_H
#ifndef SQL_H
#define SQL_H

#include <string>
#include <vector>
#include <unordered_map>
#include "cache.h"

class SQL {
public:
    SQL(Cache& cache);
    std::string executeQuery(const std::string& query);

private:
    Cache& cache;
    std::vector<std::string> parseQuery(const std::string& query);

    // Query handlers
    std::string handleSelect(const std::vector<std::string>& tokens);
    std::string handleInsert(const std::vector<std::string>& tokens);
    std::string handleUpdate(const std::vector<std::string>& tokens);
    std::string handleDelete(const std::vector<std::string>& tokens);
    std::string handleCreate(const std::vector<std::string>& tokens);
};

#endif // SQL_H

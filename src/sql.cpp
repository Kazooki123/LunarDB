#include "sql.h"
#include <sstream>
#include <algorithm>

SQL::SQL(Cache& cache) : cache(cache) {}

std::string SQL::executeQuery(const std::string& query) {
    auto tokens = parseQuery(query);
    if (tokens.empty()) {
        return "Error: Empty query";
    }

    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "SELECT") {
        return handleSelect(tokens);
    } else if (command == "INSERT") {
        return handleInsert(tokens);
    } else if (command == "UPDATE") {
        return handleUpdate(tokens);
    } else if (command == "DELETE") {
        return handleDelete(tokens);
    } else {
        return "Error: Unsupported SQL command";
    }
}

std::vector<std::string> SQL::parseQuery(const std::string& query) {
    std::vector<std::string> tokens;
    std::istringstream iss(query);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string SQL::handleSelect(const std::vector<std::string>& tokens) {
    // Implement basic SELECT functionality
    // For now, just return the value of the key
    if (tokens.size() < 4 || tokens[1] != "*" || tokens[2] != "FROM") {
        return "Error: Invalid SELECT query";
    }
    std::string key = tokens[3];
    std::string value = cache.get(key);
    return value.empty() ? "NULL" : value;
}

std::string SQL::handleInsert(const std::vector<std::string>& tokens) {
    // Implement basic INSERT functionality
    if (tokens.size() < 6 || tokens[2] != "VALUES") {
        return "Error: Invalid INSERT query";
    }
    std::string key = tokens[1];
    std::string value = tokens[3];
    cache.set(key, value);
    return "OK";
}

std::string SQL::handleUpdate(const std::vector<std::string>& tokens) {
    // Implement basic UPDATE functionality
    if (tokens.size() < 6 || tokens[2] != "SET" || tokens[4] != "WHERE") {
        return "Error: Invalid UPDATE query";
    }
    std::string key = tokens[5];
    std::string value = tokens[3];
    if (cache.del(key)) {
        cache.set(key, value);
        return "OK";
    }
    return "Error: Key not found";
}

std::string SQL::handleDelete(const std::vector<std::string>& tokens) {
    // Implement basic DELETE functionality
    if (tokens.size() < 4 || tokens[1] != "FROM" || tokens[3] != "WHERE") {
        return "Error: Invalid DELETE query";
    }
    std::string key = tokens[4];
    if (cache.del(key)) {
        return "OK";
    }
    return "Error: Key not found";
}
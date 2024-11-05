#include "sql.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <stdexcept>

// Table structure to store column definitions and data
struct Table {
    std::vector<std::string> columns;
    std::vector<std::unordered_map<std::string, std::string>> rows;
};

SQL::SQL(Cache& cache) : cache(cache) {}

std::string SQL::executeQuery(const std::string& query) {
    try {
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
        } else if (command == "CREATE") {
            return handleCreate(tokens);
        } else {
            return "Error: Unsupported SQL command";
        }
    } catch (const std::exception& e) {
        return std::string("Error: ") + e.what();
    }
}

std::vector<std::string> SQL::parseQuery(const std::string& query) {
    std::vector<std::string> tokens;
    std::regex pattern(R"(([*,()=<>!]+)|([^\s,()=<>!]+))");
    
    auto words_begin = std::sregex_iterator(query.begin(), query.end(), pattern);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        tokens.push_back((*i).str());
    }
    return tokens;
}

std::string SQL::handleCreate(const std::vector<std::string>& tokens) {
    if (tokens.size() < 4 || tokens[1] != "TABLE") {
        throw std::runtime_error("Invalid CREATE TABLE syntax");
    }

    std::string tableName = tokens[2];
    
    // Extract column definitions
    std::vector<std::string> columns;
    size_t i = 3;
    if (tokens[i] == "(") {
        i++;
        while (i < tokens.size() && tokens[i] != ")") {
            if (tokens[i] != ",") {
                columns.push_back(tokens[i]);
            }
            i++;
        }
    }

    if (columns.empty()) {
        throw std::runtime_error("No columns specified in CREATE TABLE");
    }

    // Store table definition in cache
    Table table;
    table.columns = columns;
    
    // Serialize table structure to string (simple implementation)
    std::string tableData = "COLS:";
    for (const auto& col : columns) {
        tableData += col + ",";
    }
    
    cache.set("TABLE_" + tableName, tableData);
    return "Table created successfully";
}

std::string SQL::handleSelect(const std::vector<std::string>& tokens) {
    if (tokens.size() < 4) {
        throw std::runtime_error("Invalid SELECT syntax");
    }

    std::string tableName;
    std::vector<std::string> columns;
    std::string whereColumn;
    std::string whereOperator;
    std::string whereValue;
    
    // Parse SELECT part
    size_t i = 1;
    if (tokens[i] == "*") {
        columns.push_back("*");
        i++;
    } else {
        while (i < tokens.size() && tokens[i] != "FROM") {
            if (tokens[i] != ",") {
                columns.push_back(tokens[i]);
            }
            i++;
        }
    }

    // Parse FROM part
    if (i >= tokens.size() || tokens[i] != "FROM") {
        throw std::runtime_error("Missing FROM clause");
    }
    i++;
    if (i >= tokens.size()) {
        throw std::runtime_error("Missing table name");
    }
    tableName = tokens[i];
    i++;

    // Parse WHERE clause if present
    if (i < tokens.size() && tokens[i] == "WHERE") {
        if (i + 3 >= tokens.size()) {
            throw std::runtime_error("Invalid WHERE clause");
        }
        whereColumn = tokens[i + 1];
        whereOperator = tokens[i + 2];
        whereValue = tokens[i + 3];
    }

    // Retrieve table definition
    std::string tableData = cache.get("TABLE_" + tableName);
    if (tableData.empty()) {
        throw std::runtime_error("Table not found");
    }

    // For this example, we'll just return the raw data
    // In a real implementation, you would parse the table data and apply the WHERE condition
    std::stringstream result;
    result << "SELECT Result:\n";
    result << "Columns: ";
    for (const auto& col : columns) {
        result << col << " ";
    }
    if (!whereColumn.empty()) {
        result << "\nWHERE " << whereColumn << " " << whereOperator << " " << whereValue;
    }
    return result.str();
}

std::string SQL::handleInsert(const std::vector<std::string>& tokens) {
    if (tokens.size() < 4) {
        throw std::runtime_error("Invalid INSERT syntax");
    }

    std::string tableName = tokens[1];
    
    // Check if table exists
    std::string tableData = cache.get("TABLE_" + tableName);
    if (tableData.empty()) {
        throw std::runtime_error("Table not found");
    }

    // Parse VALUES
    std::vector<std::string> values;
    size_t i = 3;  // Skip "VALUES"
    if (tokens[i] == "(") {
        i++;
        while (i < tokens.size() && tokens[i] != ")") {
            if (tokens[i] != ",") {
                values.push_back(tokens[i]);
            }
            i++;
        }
    }

    // Store the values (simplified implementation)
    std::string rowKey = tableName + "_row_" + std::to_string(rand());
    std::string rowData;
    for (const auto& val : values) {
        rowData += val + ",";
    }
    cache.set(rowKey, rowData);

    return "Insert successful";
}

std::string SQL::handleUpdate(const std::vector<std::string>& tokens) {
    if (tokens.size() < 6 || tokens[2] != "SET") {
        throw std::runtime_error("Invalid UPDATE syntax");
    }

    std::string tableName = tokens[1];
    std::string setColumn = tokens[3];
    std::string setValue = tokens[5];
    
    std::string whereColumn;
    std::string whereOperator;
    std::string whereValue;

    // Parse WHERE clause
    size_t i = 6;
    if (i < tokens.size() && tokens[i] == "WHERE") {
        if (i + 3 >= tokens.size()) {
            throw std::runtime_error("Invalid WHERE clause");
        }
        whereColumn = tokens[i + 1];
        whereOperator = tokens[i + 2];
        whereValue = tokens[i + 3];
    }

    // Simplified implementation - just confirm the operation
    std::stringstream result;
    result << "UPDATE " << tableName << " SET " << setColumn << " = " << setValue;
    if (!whereColumn.empty()) {
        result << " WHERE " << whereColumn << " " << whereOperator << " " << whereValue;
    }
    return result.str() + " - Operation simulated";
}

std::string SQL::handleDelete(const std::vector<std::string>& tokens) {
    if (tokens.size() < 4 || tokens[1] != "FROM") {
        throw std::runtime_error("Invalid DELETE syntax");
    }

    std::string tableName = tokens[2];
    std::string whereColumn;
    std::string whereOperator;
    std::string whereValue;

    // Parse WHERE clause
    if (tokens[3] == "WHERE") {
        if (tokens.size() < 7) {
            throw std::runtime_error("Invalid WHERE clause");
        }
        whereColumn = tokens[4];
        whereOperator = tokens[5];
        whereValue = tokens[6];
    }

    // Simplified implementation - just confirm the operation
    std::stringstream result;
    result << "DELETE FROM " << tableName;
    if (!whereColumn.empty()) {
        result << " WHERE " << whereColumn << " " << whereOperator << " " << whereValue;
    }
    return result.str() + " - Operation simulated";
}

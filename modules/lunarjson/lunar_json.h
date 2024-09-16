#ifndef LUNAR_JSON_H
#define LUNAR_JSON_H

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>

namespace LunarJSON {

class JSONValue;

using JSONObject = std::map<std::string, JSONValue>;
using JSONArray = std::vector<JSONValue>;

class JSONValue {
public:
    using ValueType = std::variant<std::nullptr_t, bool, int, double, std::string, JSONArray, JSONObject>;

private:
    ValueType m_value;

public:
    JSONValue() : m_value(nullptr) {}
    JSONValue(std::nullptr_t) : m_value(nullptr) {}
    JSONValue(bool value) : m_value(value) {}
    JSONValue(int value) : m_value(value) {}
    JSONValue(double value) : m_value(value) {}
    JSONValue(const std::string& value) : m_value(value) {}
    JSONValue(const JSONArray& value) : m_value(value) {}
    JSONValue(const JSONObject& value) : m_value(value) {}

    bool is_null() const { return std::holds_alternative<std::nullptr_t>(m_value); }
    bool is_bool() const { return std::holds_alternative<bool>(m_value); }
    bool is_int() const { return std::holds_alternative<int>(m_value); }
    bool is_double() const { return std::holds_alternative<double>(m_value); }
    bool is_string() const { return std::holds_alternative<std::string>(m_value); }
    bool is_array() const { return std::holds_alternative<JSONArray>(m_value); }
    bool is_object() const { return std::holds_alternative<JSONObject>(m_value); }

    // Getter methods (w/ error checkings)
    bool as_bool() const { 
        if (!is_bool()) throw std::runtime_error("JSON value is not a boolean");
        return std::get<bool>(m_value);
    }
    int as_int() const {
        if (!is_int()) throw std::runtime_error("JSON value is not an integer");
        return std::get<int>(m_value);
    }
    double as_double() const {
        if (!is_double()) throw std::runtime_error("JSON value is not a double");
        return std::get<double>(m_value);
    }
    const std::string& as_string() const {
        if (!is_string()) throw std::runtime_error("JSON value is not a string");
        return std::get<std::string>(m_value);
    }
    const JSONArray& as_array() const {
        if (!is_array()) throw std::runtime_error("JSON value is not an array");
        return std::get<JSONArray>(m_value);
    }
    const JSONObject& as_object() const {
        if (!is_object()) throw std::runtime_error("JSON value is not an object");
        return std::get<JSONObject>(m_value);
    }

    // Serialization
    std::string to_string() const;
};

// Parse JSON string
JSONValue parse(const std::string& json_str);

// Stringify JSON value
std::string stringify(const JSONValue& value);

}

#endif // LUNAR_JSON_H
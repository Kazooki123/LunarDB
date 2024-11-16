#include <string>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <chrono>
#include <iomanip>
#include "../cache.h"

class APIHandler {
private:
    Cache& cache;
    std::unordered_map<std::string, std::function<std::string(std::istringstream&)>> handlers;

    // Helper to measure execution time like in main.cpp
    template<typename Func>
    double measureExecutionTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0;
    }

public:
    explicit APIHandler(Cache& cache_instance) : cache(cache_instance) {
        // Initialize command handlers similar to main.cpp
        handlers["SET"] = [this](std::istringstream& iss) {
            std::string key, value;
            int ttl = 0;
            if (iss >> key >> value) {
                iss >> ttl;
                double execTime = measureExecutionTime([&]() {
                    cache.set(key, value, ttl);
                });
                std::ostringstream response;
                response << "OK (" << std::fixed << std::setprecision(3) << execTime << "ms)";
                return response.str();
            }
            return "Invalid SET command";
        };

        handlers["GET"] = [this](std::istringstream& iss) {
            std::string key;
            if (iss >> key) {
                std::string result;
                double execTime = measureExecutionTime([&]() {
                    result = cache.get(key);
                });
                if (!result.empty()) {
                    std::ostringstream response;
                    response << result << " (" << std::fixed << std::setprecision(3) << execTime << "ms)";
                    return response.str();
                }
                return "Key not found";
            }
            return "Invalid GET command";
        };
    }

    std::string handleRequest(const std::string& request) {
        std::istringstream iss(request);
        std::string command;
        iss >> command;

        auto handler = handlers.find(command);
        if (handler != handlers.end()) {
            return handler->second(iss);
        }

        return "Unknown command";
    }
};

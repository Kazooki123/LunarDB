#ifndef RATE_LIMITING_H
#define RATE_LIMITING_H

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <string>

class RateLimiting {
public:
    // Constructor with configurable window size and request limit
    RateLimiting(std::chrono::seconds windowSize = std::chrono::seconds(60),
                 size_t maxRequests = 100)
        : windowSize_(windowSize), maxRequests_(maxRequests) {}

    // Check if a request from a client should be allowed
    bool shouldAllowRequest(const std::string& clientId) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();

        // Clean up expired entries
        cleanupExpiredEntries(now);

        // Get or create client entry
        auto& clientData = requestCounts_[clientId];

        // Remove requests outside the window
        while (!clientData.requestTimes.empty() &&
               now - clientData.requestTimes.front() > windowSize_) {
            clientData.requestTimes.pop_front();
        }

        // Check if limit exceeded
        if (clientData.requestTimes.size() >= maxRequests_) {
            return false;
        }

        // Add new request
        clientData.requestTimes.push_back(now);
        return true;
    }

    // Reset rate limits for a client
    void resetClient(const std::string& clientId) {
        std::lock_guard<std::mutex> lock(mutex_);
        requestCounts_.erase(clientId);
    }

    // Get remaining requests for a client
    size_t getRemainingRequests(const std::string& clientId) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();
        cleanupExpiredEntries(now);

        auto it = requestCounts_.find(clientId);
        if (it == requestCounts_.end()) {
            return maxRequests_;
        }

        return maxRequests_ - it->second.requestTimes.size();
    }

private:
    struct ClientData {
        std::deque<std::chrono::steady_clock::time_point> requestTimes;
    };

    void cleanupExpiredEntries(const std::chrono::steady_clock::time_point& now) {
        for (auto it = requestCounts_.begin(); it != requestCounts_.end();) {
            auto& times = it->second.requestTimes;
            while (!times.empty() && now - times.front() > windowSize_) {
                times.pop_front();
            }
            if (times.empty()) {
                it = requestCounts_.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::chrono::seconds windowSize_;
    size_t maxRequests_;
    std::unordered_map<std::string, ClientData> requestCounts_;
    std::mutex mutex_;
};

#endif // RATE_LIMITING_H

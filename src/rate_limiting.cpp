#include "rate_limiting.h"
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <algorithm>

// Thread-safe rate limiter implementation
RateLimiter::RateLimiter(size_t tokens, std::chrono::milliseconds window)
    : max_tokens_(tokens)
    , time_window_(window)
{
}

bool RateLimiter::try_acquire(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = std::chrono::steady_clock::now();
    auto& bucket = buckets_[key];

    // Remove expired tokens
    bucket.tokens.erase(
        std::remove_if(bucket.tokens.begin(), bucket.tokens.end(),
            [&](const auto& timestamp) {
                return now - timestamp > time_window_;
            }
        ),
        bucket.tokens.end()
    );

    // Check if we can add a new token
    if (bucket.tokens.size() < max_tokens_) {
        bucket.tokens.push_back(now);
        return true;
    }

    return false;
}

void RateLimiter::reset(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    buckets_.erase(key);
}

void RateLimiter::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    buckets_.clear();
}

size_t RateLimiter::get_remaining_tokens(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = std::chrono::steady_clock::now();
    auto& bucket = buckets_[key];

    // Remove expired tokens first
    bucket.tokens.erase(
        std::remove_if(bucket.tokens.begin(), bucket.tokens.end(),
            [&](const auto& timestamp) {
                return now - timestamp > time_window_;
            }
        ),
        bucket.tokens.end()
    );

    return max_tokens_ - bucket.tokens.size();
}

bool RateLimiter::is_rate_limited(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = std::chrono::steady_clock::now();
    auto& bucket = buckets_[key];

    // Remove expired tokens
    bucket.tokens.erase(
        std::remove_if(bucket.tokens.begin(), bucket.tokens.end(),
            [&](const auto& timestamp) {
                return now - timestamp > time_window_;
            }
        ),
        bucket.tokens.end()
    );

    return bucket.tokens.size() >= max_tokens_;
}

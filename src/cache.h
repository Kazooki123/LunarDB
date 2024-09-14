#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <utility>

class Cache {
private:
    struct CacheEntry {
        std::string value;
        std::chrono::steady_clock::time_point expiry;
        bool has_expiry;
    };

    std::unordered_map<std::string, CacheEntry> data;
    size_t max_size;

    void evict_if_needed();

public:
    Cache(size_t max_size = 1000);

    void set(const std::string& key, const std::string& value, int ttl_seconds = 0);
    std::string get(const std::string& key) const;  // Changed to const
    bool del(const std::string& key);
    void clear();
    size_t size() const;

    // Batch operations
    void mset(const std::vector<std::pair<std::string, std::string>>& kvs);
    std::vector<std::string> mget(const std::vector<std::string>& keys) const;  // Changed to const

    // Utility methods
    std::vector<std::string> keys() const;  // Already const
    void cleanup_expired();
};

#endif // CACHE_H
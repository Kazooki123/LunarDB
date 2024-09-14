#include "cache.h"
#include <algorithm>

Cache::Cache(size_t max_size) : max_size(max_size) {}

void Cache::set(const std::string& key, const std::string& value, int ttl_seconds) {
    evict_if_needed();
    bool has_expiry = (ttl_seconds > 0);
    auto expiry = std::chrono::steady_clock::now() + std::chrono::seconds(ttl_seconds);
    data[key] = {value, expiry, has_expiry};
}

std::string Cache::get(const std::string& key) const {  // Changed to const
    auto it = data.find(key);
    if (it != data.end()) {
        if (!it->second.has_expiry || it->second.expiry > std::chrono::steady_clock::now()) {
            return it->second.value;
        }
    }
    return "";
}

bool Cache::del(const std::string& key) {
    return data.erase(key) > 0;
}

void Cache::clear() {
    data.clear();
}

size_t Cache::size() const {
    return data.size();
}

void Cache::mset(const std::vector<std::pair<std::string, std::string>>& kvs) {
    for (const auto& kv : kvs) {
        set(kv.first, kv.second);
    }
}

std::vector<std::string> Cache::mget(const std::vector<std::string>& keys) const {  // Changed to const
    std::vector<std::string> results;
    results.reserve(keys.size());
    for (const auto& key : keys) {
        results.push_back(get(key));
    }
    return results;
}

std::vector<std::string> Cache::keys() const {
    std::vector<std::string> result;
    result.reserve(data.size());
    for (const auto& entry : data) {
        result.push_back(entry.first);
    }
    return result;
}

void Cache::cleanup_expired() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = data.begin(); it != data.end();) {
        if (it->second.has_expiry && it->second.expiry <= now) {
            it = data.erase(it);
        } else {
            ++it;
        }
    }
}

void Cache::evict_if_needed() {
    if (data.size() >= max_size) {
        auto oldest = std::min_element(data.begin(), data.end(),
            [](const auto& a, const auto& b) {
                return a.second.expiry < b.second.expiry;
            });
        data.erase(oldest);
    }
}
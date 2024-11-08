/*
** Copyright 2024 Kazooki123
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without 
** limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies     
** of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following 
** conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial 
** portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
** LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
** THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
** 
**/

#include "cache.h"
#include <algorithm>

Cache::Cache(size_t max_size) : max_size(max_size) {}

void Cache::set(const std::string &key, const std::string &value, int ttl_seconds) {
    evict_if_needed();
    CacheEntry entry;
    entry.is_list = false;
    new (&entry.string_value) std::string(value);
    entry.has_expiry = (ttl_seconds > 0);
    entry.expiry = std::chrono::steady_clock::now() + std::chrono::seconds(ttl_seconds);
    data[key] = std::move(entry);
    
    // Sync with provider if attached
    syncWithProvider(key, data[key]);
}

std::string Cache::get(const std::string &key) const {
    auto it = data.find(key);
    if (it != data.end()) {
        if (!it->second.has_expiry || it->second.expiry > std::chrono::steady_clock::now()) {
            return it->second.is_list ? "" : it->second.string_value;
        }
    }
    
    // If not found in cache and provider exists, try to get from provider
    if (provider_) {
        auto value = provider_->get(key);
        if (value.has_value()) {
            // Update cache with provider value
            const_cast<Cache*>(this)->set(key, *value);
            return *value;
        }
    }
    return "";
}

bool Cache::del(const std::string &key) {
    bool deleted = data.erase(key) > 0;
    if (deleted && provider_) {
        provider_->del(key);
    }
    return deleted;
}

void Cache::clear() {
    data.clear();
}

size_t Cache::size() const {
    return data.size();
}

void Cache::mset(const std::vector<std::pair<std::string, std::string>> &kvs) {
    for (const auto &kv : kvs) {
        set(kv.first, kv.second);
    }
}

std::vector<std::string> Cache::mget(const std::vector<std::string> &keys) const {
    std::vector<std::string> results;
    results.reserve(keys.size());
    for (const auto &key : keys) {
        results.push_back(get(key));
    }
    return results;
}

std::vector<std::string> Cache::keys() const {
    std::vector<std::string> result;
    result.reserve(data.size());
    for (const auto &entry : data) {
        result.push_back(entry.first);
    }
    return result;
}

void Cache::cleanup_expired() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = data.begin(); it != data.end();) {
        if (it->second.has_expiry && it->second.expiry <= now) {
            if (provider_) {
                provider_->del(it->first);
            }
            it = data.erase(it);
        } else {
            ++it;
        }
    }
}

void Cache::evict_if_needed() {
    if (data.size() >= max_size) {
        auto oldest = std::min_element(data.begin(), data.end(),
                                     [](const auto &a, const auto &b) {
                                         return a.second.expiry < b.second.expiry;
                                     });
        if (provider_) {
            provider_->del(oldest->first);
        }
        data.erase(oldest);
    }
}

void Cache::lpush(const std::string& key, const std::string& value) {
    auto it = data.find(key);
    if (it == data.end() || !it->second.is_list) {
        CacheEntry entry;
        entry.is_list = true;
        new (&entry.list_value) std::list<std::string>{value};
        entry.has_expiry = false;
        data[key] = std::move(entry);
    } else {
        it->second.list_value.push_front(value);
    }
    
    // Sync with provider
    syncWithProvider(key, data[key]);
}

std::string Cache::lpop(const std::string& key) {
    auto it = data.find(key);
    if (it != data.end() && it->second.is_list) {
        auto& list = it->second.list_value;
        if (!list.empty()) {
            std::string value = list.front();
            list.pop_front();
            if (list.empty()) {
                if (provider_) {
                    provider_->del(key);
                }
                data.erase(it);
            } else {
                syncWithProvider(key, it->second);
            }
            return value;
        }
    }
    return "";
}

void Cache::rpush(const std::string& key, const std::string& value) {
    auto it = data.find(key);
    if (it == data.end() || !it->second.is_list) {
        CacheEntry entry;
        entry.is_list = true;
        new (&entry.list_value) std::list<std::string>{value};
        entry.has_expiry = false;
        data[key] = std::move(entry);
    } else {
        it->second.list_value.push_back(value);
    }
    
    // Sync with provider
    syncWithProvider(key, data[key]);
}

std::string Cache::rpop(const std::string& key) {
    auto it = data.find(key);
    if (it != data.end() && it->second.is_list) {
        auto& list = it->second.list_value;
        if (!list.empty()) {
            std::string value = list.back();
            list.pop_back();
            if (list.empty()) {
                if (provider_) {
                    provider_->del(key);
                }
                data.erase(it);
            } else {
                syncWithProvider(key, it->second);
            }
            return value;
        }
    }
    return "";
}

std::vector<std::string> Cache::lrange(const std::string& key, int start, int stop) const {
    auto it = data.find(key);
    if (it != data.end() && it->second.is_list) {
        const auto& list = it->second.list_value;
        std::vector<std::string> result;
        int size = static_cast<int>(list.size());

        if (start < 0) start = size + start;
        if (stop < 0) stop = size + stop;

        start = std::max(0, std::min(start, size - 1));
        stop = std::max(0, std::min(stop, size - 1));

        auto it_start = std::next(list.begin(), start);
        auto it_stop = std::next(list.begin(), stop + 1);

        result.insert(result.end(), it_start, it_stop);
        return result;
    }
    return {};
}

size_t Cache::llen(const std::string& key) const {
    auto it = data.find(key);
    if (it != data.end() && it->second.is_list) {
        return it->second.list_value.size();
    }
    return 0;
}
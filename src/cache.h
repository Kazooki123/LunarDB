#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <unordered_map>
#include <list>
#include <chrono>
#include <vector>
#include <utility>

class Cache {
private:
  struct CacheEntry {
        union {
            std::string string_value;
            std::list<std::string> list_value;
        };
        bool is_list;
        std::chrono::steady_clock::time_point expiry;
        bool has_expiry;

        CacheEntry() : is_list(false), has_expiry(false) {
            new (&string_value) std::string();
        }

        CacheEntry(const CacheEntry& other) : is_list(other.is_list), expiry(other.expiry), has_expiry(other.has_expiry) {
            if (is_list) {
                new (&list_value) std::list<std::string>(other.list_value);
            } else {
                new (&string_value) std::string(other.string_value);
            }
        }

        CacheEntry(CacheEntry&& other) noexcept : is_list(other.is_list), expiry(other.expiry), has_expiry(other.has_expiry) {
            if (is_list) {
                new (&list_value) std::list<std::string>(std::move(other.list_value));
            } else {
                new (&string_value) std::string(std::move(other.string_value));
            }
        }

        CacheEntry& operator=(const CacheEntry& other) {
            if (this != &other) {
                this->~CacheEntry();
                new (this) CacheEntry(other);
            }
            return *this;
        }

        CacheEntry& operator=(CacheEntry&& other) noexcept {
            if (this != &other) {
                this->~CacheEntry();
                new (this) CacheEntry(std::move(other));
            }
            return *this;
        }

        ~CacheEntry() {
            if (is_list) {
                list_value.~list();
            } else {
                string_value.~basic_string();
            }
        }
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

    // (New) List methods
    void lpush(const std::string& key, const std::string& value);
    std::string lpop(const std::string& key);
    void rpush(const std::string& key, const std::string& value);
    std::string rpop(const std::string& key);
    std::vector<std::string> lrange(const std::string& key, int start, int stop) const;
    size_t llen(const std::string& key) const;
};

/*
** Copyright 2024 Kazooki123
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the “Software”), to deal in the Software without restriction, including without 
** limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies     
** of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following 
** conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial 
** portions of the Software.
** 
** THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
** LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
** THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
** 
**/

#endif // CACHE_H
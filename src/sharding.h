#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <stdexcept>

namespace LunarDB {

class Shard {
public:
    virtual ~Shard() = default;
    virtual void insert(const std::string& key, const std::string& value) = 0;
    virtual std::optional<std::string> get(const std::string& key) const = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual size_t size() const = 0;
    virtual std::vector<std::pair<std::string, std::string>> getAll() const = 0;
};

class ShardManager {
public:
    ShardManager(
        size_t numShards = 4,
        std::hash<std::string> hasher = std::hash<std::string>()
    );

    void insert(const std::string& key, const std::string& value, bool overwrite = true);

    std::optional<std::string> get(const std::string& key) const;

    bool remove(const std::string& key);

    size_t getShardIndex(const std::string& key) const;
    size_t getShardCount() const { return shards_.size(); }

    size_t getTotalKeyCount() const;

    void rebalance(size_t newShardCount);

    template<typename Func>
    void forEachShard(Func&& func);

private:
    std::vector<std::unique_ptr<Shard>> shards_;
    std::hash<std::string> hasher_;
    mutable std::shared_mutex shards_mutex_;
};

class MemoryShard : public Shard {
public:
    void insert(const std::string& key, const std::string& value) override;
    std::optional<std::string> get(const std::string& key) const override;
    bool remove(const std::string& key) override;
    size_t size() const override { return data_.size(); }
    std::vector<std::pair<std::string, std::string>> getAll() const override;

private:
    std::unordered_map<std::string, std::string> data_;
    mutable std::shared_mutex data_mutex_;
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

}

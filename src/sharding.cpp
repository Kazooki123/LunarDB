// Authors: Kazooki123, StarloExoliz

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

#include "sharding.h"
#include <stdexcept>
#include <shared_mutex>
#include <algorithm>
#include <mutex>

namespace LunarDB {

ShardManager::ShardManager(size_t numShards, std::hash<std::string> hasher)
    : hasher_(hasher) {
    if (numShards == 0) {
        throw std::invalid_argument("Number of shards must be greater than 0");
    }
    shards_.reserve(numShards);
    for (size_t i = 0; i < numShards; ++i) {
        shards_.push_back(std::make_unique<MemoryShard>());
    }
}

void ShardManager::insert(const std::string& key, const std::string& value, bool overwrite) {
    size_t shardIndex = getShardIndex(key);

    std::unique_lock<std::shared_mutex> lock(shards_mutex_);
    shards_[shardIndex]->insert(key, value);
}

std::optional<std::string> ShardManager::get(const std::string& key) const {
    size_t shardIndex = getShardIndex(key);

    std::shared_lock<std::shared_mutex> lock(shards_mutex_);
    return shards_[shardIndex]->get(key);
}

bool ShardManager::remove(const std::string& key) {
    size_t shardIndex = getShardIndex(key);

    std::unique_lock<std::shared_mutex> lock(shards_mutex_);
    return shards_[shardIndex]->remove(key);
}

size_t ShardManager::getShardIndex(const std::string& key) const {
    return hasher_(key) % shards_.size();
}

size_t ShardManager::getTotalKeyCount() const {
    std::shared_lock<std::shared_mutex> lock(shards_mutex_);
    size_t totalKeys = 0;
    for (const auto& shard : shards_) {
        totalKeys += shard->size();
    }
    return totalKeys;
}

void ShardManager::rebalance(size_t newShardCount) {
    if (newShardCount == 0) {
        throw std::invalid_argument("Number of shards must be greater than 0");
    }

    std::unique_lock<std::shared_mutex> lock(shards_mutex_);

    // Temporary vector to hold all keys
    std::vector<std::pair<std::string, std::string>> allData;

    // Extract all data from current shards
    for (const auto& shard : shards_) {
        auto shardData = shard->getAll();
        allData.insert(allData.end(), shardData.begin(), shardData.end());
    }

    // Clear existing shards
    shards_.clear();
    shards_.reserve(newShardCount);

    // Create new shards
    for (size_t i = 0; i < newShardCount; ++i) {
        shards_.push_back(std::make_unique<MemoryShard>());
    }

    // Redistribute data to new shards
    for (const auto& [key, value] : allData) {
        insert(key, value);
    }
}

template<typename Func>
void ShardManager::forEachShard(Func&& func) {
    std::shared_lock<std::shared_mutex> lock(shards_mutex_);
    for (auto& shard : shards_) {
        func(*shard);
    }
}

void MemoryShard::insert(const std::string& key, const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);
    data_[key] = value;
}

std::optional<std::string> MemoryShard::get(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);
    auto it = data_.find(key);
    return it != data_.end() ? std::optional<std::string>(it->second) : std::nullopt;
}

bool MemoryShard::remove(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);
    return data_.erase(key) > 0;
}

std::vector<std::pair<std::string, std::string>> MemoryShard::getAll() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(data_.size());

    for (const auto& [key, value] : data_) {
        result.push_back({key, value});
    }

    return result;
}

}  // namespace LunarDB

#include "sharding.h"
#include <stdexcept>

namespace LunarDB {

ShardManager::ShardManager(size_t numShards) {
    if (numShards == 0) {
        throw std::invalid_argument("Number of shards must be greater than 0");
    }
    shards_.reserve(numShards);
    for (size_t i = 0; i < numShards; ++i) {
        shards_.push_back(std::make_unique<MemoryShard>());
    }
}

void ShardManager::insert(const std::string& key, const std::string& value) {
    size_t shardIndex = getShardIndex(key);
    shards_[shardIndex]->insert(key, value);
}

std::optional<std::string> ShardManager::get(const std::string& key) const {
    size_t shardIndex = getShardIndex(key);
    return shards_[shardIndex]->get(key);
}

bool ShardManager::remove(const std::string& key) {
    size_t shardIndex = getShardIndex(key);
    return shards_[shardIndex]->remove(key);
}

size_t ShardManager::getShardIndex(const std::string& key) const {
    return hasher_(key) % shards_.size();
}

void MemoryShard::insert(const std::string& key, const std::string& value) {
    data_[key] = value;
}

std::optional<std::string> MemoryShard::get(const std::string& key) const {
    auto it = data_.find(key);
    if (it != data_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool MemoryShard::remove(const std::string& key) {
    return data_.erase(key) > 0;
}

}

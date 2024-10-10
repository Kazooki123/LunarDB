#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <optional>

namespace LunarDB {

class Shard {
public:
    virtual ~Shard() = default;
    virtual void insert(const std::string& key, const std::string& value) = 0;
    virtual std::optional<std::string> get(const std::string& key) const = 0;
    virtual bool remove(const std::string& key) = 0;
};

class ShardManager {
public:
    ShardManager(size_t numShards);

    void insert(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    bool remove(const std::string& key);

    size_t getShardIndex(const std::string& key) const;
    size_t getShardCount() const { return shards_.size(); }

private:
    std::vector<std::unique_ptr<Shard>> shards_;
    std::hash<std::string> hasher_;
};

class MemoryShard : public Shard {
public:
    void insert(const std::string& key, const std::string& value) override;
    std::optional<std::string> get(const std::string& key) const override;
    bool remove(const std::string& key) override;

private:
    std::unordered_map<std::string, std::string> data_;
};

}

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


#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace lunardb {
namespace providers {

struct ProviderConfig {
    std::string host;
    int port;
    std::string username;
    std::string password;
    std::string database;
};

class Provider {
public:
    virtual ~Provider() = default;
    
    // Connection management
    virtual bool connect(const ProviderConfig& config) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // Basic operations
    virtual bool set(const std::string& key, const std::string& value, int ttl = 0) = 0;
    virtual std::optional<std::string> get(const std::string& key) = 0;
    virtual bool del(const std::string& key) = 0;
    
    // Batch operations
    virtual bool mset(const std::vector<std::pair<std::string, std::string>>& kvs) = 0;
    virtual std::vector<std::string> mget(const std::vector<std::string>& keys) = 0;
    
    // Provider-specific operations
    virtual bool executeRaw(const std::string& query, std::string& result) = 0;
    
    // Persistence operations
    virtual bool sync() = 0;
    virtual bool backup(const std::string& path) = 0;
    virtual bool restore(const std::string& path) = 0;
};

// Factory function type for creating providers
using ProviderFactory = std::unique_ptr<Provider>(*)();

} // namespace PROVIDERS
} // namespace LUNARDB
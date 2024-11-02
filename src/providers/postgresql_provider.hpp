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

#include "provider.hpp"
#include <pqxx/pqxx>
#include <mutex>

namespace lunardb {
namespace providers {

class PostgreSQLProvider : public Provider {
public:
    PostgreSQLProvider();
    ~PostgreSQLProvider() override;

    bool connect(const ProviderConfig& config) override;
    void disconnect() override;
    bool isConnected() const override;
    
    bool set(const std::string& key, const std::string& value, int ttl = 0) override;
    std::optional<std::string> get(const std::string& key) override;
    bool del(const std::string& key) override;
    
    bool mset(const std::vector<std::pair<std::string, std::string>>& kvs) override;
    std::vector<std::string> mget(const std::vector<std::string>& keys) override;
    
    bool executeRaw(const std::string& query, std::string& result) override;
    
    bool sync() override;
    bool backup(const std::string& path) override;
    bool restore(const std::string& path) override;

private:
    std::unique_ptr<pqxx::connection> conn_;
    mutable std::mutex mutex_;
    
    bool ensureSchema();
    bool createTablesIfNotExist();
};

} // namespace PROVIDERS
} // namespace LUNARDB
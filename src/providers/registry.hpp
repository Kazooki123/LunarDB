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
#include <unordered_map>
#include <string>

namespace lunardb {
namespace providers {

class ProviderRegistry {
public:
    static ProviderRegistry& instance() {
        static ProviderRegistry registry;
        return registry;
    }
    
    void registerProvider(const std::string& name, ProviderFactory factory) {
        factories_[name] = factory;
    }
    
    std::unique_ptr<Provider> createProvider(const std::string& name) {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    std::vector<std::string> getAvailableProviders() const {
        std::vector<std::string> providers;
        for (const auto& [name, _] : factories_) {
            providers.push_back(name);
        }
        return providers;
    }

private:
    ProviderRegistry() = default;
    std::unordered_map<std::string, ProviderFactory> factories_;
};

} // namespace PROVIDERS
} // namespace LUNARDB
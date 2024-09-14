#include "server.h"

Server::Server() {}

void Server::set(const std::string& key, const std::string& value) {
    cache[key] = value;
}

std::string Server::get(const std::string& key) {
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second;
    }
    return "";
}

bool Server::del(const std::string& key) {
    return cache.erase(key) > 0;
}

void Server::clear() {
    cache.clear();
}

size_t Server::size() const {
    return cache.size();
}
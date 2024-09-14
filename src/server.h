#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <unordered_map>

class Server {
private:
    std::unordered_map<std::string, std::string> cache;

public:
    Server();
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    bool del(const std::string& key);
    void clear();
    size_t size() const;
};

#endif // SERVER_H
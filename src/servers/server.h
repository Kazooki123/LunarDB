#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <optional>
#include <variant>
#include "../cache.h"

class Server {
public:
    // Use string_view for const string references
    Server(std::string_view host, uint16_t port);
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    [[nodiscard]] bool start();
    void stop();
    void setMaxClients(size_t max_clients);
    [[nodiscard]] bool loadConfig(std::string_view config_file);

private:
    void handleClient(int client_socket);
    [[nodiscard]] std::string processCommand(std::string_view command);
    void acceptClients();

    // Command result type using std::variant
    using CommandResult = std::variant<std::string, std::vector<std::string>, int64_t>;
    
    const std::string host_;
    const uint16_t port_;
    int server_socket_;
    std::atomic<bool> running_;
    std::atomic<size_t> max_clients_;
    
    std::vector<std::thread> client_threads_;
    std::mutex cache_mutex_;
    
    Cache cache_;
    
    // Modern command handler type using string_view
    std::unordered_map<std::string, 
        std::function<CommandResult(const std::vector<std::string_view>&)>> command_handlers_;
    
    void initializeCommandHandlers();
};
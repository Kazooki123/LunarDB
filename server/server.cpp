#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <filesystem>
#include <charconv>

namespace fs = std::filesystem;

Server::Server(std::string_view host, uint16_t port)
    : host_(host)
    , port_(port)
    , server_socket_(-1)
    , running_(false)
    , max_clients_(100) {
    initializeCommandHandlers();
}

Server::~Server() {
    stop();
}

bool Server::start() {
    server_socket_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_socket_ == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    const int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options" << std::endl;
        return false;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    if (auto result = inet_pton(AF_INET, host_.c_str(), &server_addr.sin_addr); result <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        return false;
    }

    if (bind(server_socket_, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind to port " << port_ << std::endl;
        return false;
    }

    if (listen(server_socket_, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return false;
    }

    running_ = true;
    std::thread{&Server::acceptClients, this}.detach();

    std::cout << "LunarDB server listening on " << host_ << ":" << port_ << std::endl;
    return true;
}

void Server::stop() {
    running_ = false;
    if (server_socket_ != -1) {
        close(server_socket_);
        server_socket_ = -1;
    }
    
    for (auto& thread : client_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    client_threads_.clear();
}

void Server::acceptClients() {
    while (running_) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        if (int client_socket = accept4(server_socket_, 
                                      reinterpret_cast<struct sockaddr*>(&client_addr), 
                                      &client_len, 
                                      SOCK_NONBLOCK); client_socket >= 0) {
            if (client_threads_.size() >= max_clients_) {
                std::string_view msg = "Server at maximum capacity\n";
                send(client_socket, msg.data(), msg.size(), 0);
                close(client_socket);
                continue;
            }

            client_threads_.emplace_back(&Server::handleClient, this, client_socket);
        } else if (errno != EWOULDBLOCK && errno != EAGAIN) {
            if (running_) {
                std::cerr << "Failed to accept client connection: " << strerror(errno) << std::endl;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Server::handleClient(int client_socket) {
    std::array<char, 4096> buffer;
    std::string accumulated_data;

    while (running_) {
        ssize_t bytes_received = recv(client_socket, buffer.data(), buffer.size() - 1, 0);
        if (bytes_received <= 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        buffer[bytes_received] = '\0';
        accumulated_data.append(buffer.data(), bytes_received);

        // Process complete commands using string_view for efficient parsing
        size_t pos = 0;
        while ((pos = accumulated_data.find('\n')) != std::string::npos) {
            std::string_view command{accumulated_data.data(), pos};
            auto response = processCommand(command);
            response += '\n';
            send(client_socket, response.data(), response.size(), 0);
            accumulated_data.erase(0, pos + 1);
        }
    }

    close(client_socket);
}

std::string Server::processCommand(std::string_view command) {
    std::vector<std::string_view> args;
    size_t pos = 0, prev = 0;
    
    // Split command into arguments using string_view
    while ((pos = command.find(' ', prev)) != std::string_view::npos) {
        if (pos > prev) {
            args.emplace_back(command.substr(prev, pos - prev));
        }
        prev = pos + 1;
    }
    if (prev < command.length()) {
        args.emplace_back(command.substr(prev));
    }

    if (args.empty()) {
        return "ERR empty command";
    }

    // Convert command to uppercase using string_view
    std::string cmd{args[0]};
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    args.erase(args.begin());

    if (auto it = command_handlers_.find(cmd); it != command_handlers_.end()) {
        try {
            auto result = it->second(args);
            return std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    std::string output;
                    for (const auto& s : arg) {
                        output += s + "\n";
                    }
                    return output;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    return std::to_string(arg);
                }
            }, result);
        } catch (const std::exception& e) {
            return "ERR " + std::string(e.what());
        }
    }
    
    return "ERR Unknown command";
}

void Server::initializeCommandHandlers() {
    command_handlers_["SET"] = [this](const std::vector<std::string_view>& args) -> CommandResult {
        if (args.size() < 2) {
            throw std::runtime_error("wrong number of arguments for 'set' command");
        }
        std::lock_guard<std::mutex> lock(cache_mutex_);
        int ttl = args.size() > 2 ? std::stoi(std::string(args[2])) : 0;
        cache_.set(std::string(args[0]), std::string(args[1]), ttl);
        return "OK";
    };

    command_handlers_["GET"] = [this](const std::vector<std::string_view>& args) -> CommandResult {
        if (args.empty()) {
            throw std::runtime_error("wrong number of arguments for 'get' command");
        }
        std::lock_guard<std::mutex> lock(cache_mutex_);
        std::string value = cache_.get(std::string(args[0]));
        return value.empty() ? "(nil)" : value;
    };
}

bool Server::loadConfig(std::string_view config_file) {
    std::ifstream file{std::string(config_file)};  // Fix vexing parse by using curly braces

    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file: " << config_file << std::endl;
        return false;
    }

    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errs;

    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        std::cerr << "Error: Failed to parse config file: " << errs << std::endl;
        return false;
    }

    // Process `root` as needed
    return true;
}


void Server::setMaxClients(size_t max_clients) {
    max_clients_ = max_clients;
}
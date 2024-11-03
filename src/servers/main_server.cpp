#include "server.h"
#include <iostream>
#include <csignal>
#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;
namespace {
    std::unique_ptr<Server> server;

    void signal_handler(int signum) {
        if (server) {
            std::cout << "\nShutting down LunarDB server..." << std::endl;
            server->stop();
        }
        std::exit(signum);
    }

    struct ServerConfig {
        std::string host = "127.0.0.1";
        uint16_t port = 6379;  // Same default port as Redis
        std::string config_file = "../src/servers/lunardb.json";
    };

    [[nodiscard]] ServerConfig parse_arguments(int argc, char* argv[]) {
        ServerConfig config;
        
        for (int i = 1; i < argc; i++) {
            std::string_view arg(argv[i]);
            
            if (arg == "--host" && i + 1 < argc) {
                config.host = argv[++i];
            } else if (arg == "--port" && i + 1 < argc) {
                config.port = static_cast<uint16_t>(std::stoi(argv[++i]));
            } else if (arg == "--config" && i + 1 < argc) {
                config.config_file = argv[++i];
            }
        }
        
        return config;
    }
}

int main(int argc, char* argv[]) {
    try {
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        auto config = parse_arguments(argc, argv);

        server = std::make_unique<Server>(config.host, config.port);

        if (fs::exists(config.config_file)) {
            if (server->loadConfig(config.config_file)) {
                std::cout << "Configuration loaded from " << config.config_file << std::endl;
            } else {
                std::cerr << "Failed to load configuration from " << config.config_file << std::endl;
            }
        }

        if (!server->start()) {
            std::cerr << "Failed to start server" << std::endl;
            return 1;
        }

        // Keep the main thread alive
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
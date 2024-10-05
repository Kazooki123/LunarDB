#include "server.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

LunarDBServer::LunarDBServer() {
    // Initialize any necessary components
    // Empty for now
}

void LunarDBServer::serve(const std::string& ip, int port) {
    std::string serverIP = ip.empty() ? getLocalIP() : ip;
    int serverPort = port == 0 ? findAvailablePort() : port;

    std::cout << "Running locally...\n";
    std::cout << "LunarDB server has been launched!\n";
    std::cout << "IP: " << serverIP << "\n";
    std::cout << "PORT: " << serverPort << "\n";

    // Here you would implement the actual server logic
    // For now, we'll just keep the program running
    while (true) {
        // Server loops
    }
}

std::string LunarDBServer::getLocalIP() const {
    // In a real implementation, you'd use platform-specific code to get the local IP
    // For this example, we'll return a placeholder
    return "127.0.0.1";
}

int LunarDBServer::findAvailablePort() const {
    // In a real implementation, you'd search for an available port
    // For this example, we'll return a random port number between 1024 and 65535
    std::srand(std::time(nullptr));
    return 1024 + (std::rand() % (65535 - 1024 + 1));
}

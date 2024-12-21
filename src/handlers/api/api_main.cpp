#include "api_handler.h"
#include "../../cache.h"
#include <memory>
#include <iostream>
#include <signal.h>

std::unique_ptr<lunardb::api::APIHandler> api_handler;

void signal_handler(int signum) {
    std::cout << "\nReceived signal " << signum << ". Shutting down..." << std::endl;
    if (api_handler) {
        api_handler->stop();
    }
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    auto cache = std::make_shared<Cache>();

    // Create and start API handler
    api_handler = std::make_unique<lunardb::api::APIHandler>(cache);

    std::cout << "Starting LunarDB API server on http://0.0.0.0:18080" << std::endl;
    api_handler->start();

    return 0;
}

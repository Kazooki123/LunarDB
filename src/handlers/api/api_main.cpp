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

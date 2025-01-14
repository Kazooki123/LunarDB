#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "../../cache.h"
#include <crow.h>
#include <memory>
#include <string>

namespace lunardb {
namespace api {

class APIHandler {
private:
    std::shared_ptr<Cache> cache_;
    crow::SimpleApp app_;

    // Utility methods
    crow::response createJsonResponse(int status, const std::string& message, crow::json::wvalue data = {});
    bool validateKey(const std::string& key, crow::response& error);

    // Route handlers
    void setupRoutes();
    crow::response handleGet(const crow::request& req, const std::string& key);
    crow::response handleSet(const crow::request& req);
    crow::response handleDelete(const std::string& key);
    crow::response handleMget(const crow::request& req);
    crow::response handleMset(const crow::request& req);
    crow::response handleKeys();

    // List operations handlers
    crow::response handleLpush(const crow::request& req);
    crow::response handleRpush(const crow::request& req);
    crow::response handleLpop(const std::string& key);
    crow::response handleRpop(const std::string& key);
    crow::response handleLrange(const crow::request& req);
    crow::response handleLlen(const std::string& key);

public:
    explicit APIHandler(std::shared_ptr<Cache> cache);

    void start(const std::string& host = "0.0.0.0", uint16_t port = 18080);
    void stop();
    void save();
};

} // namespace api
} // namespace lunardb

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

#endif // API_HANDLER_H

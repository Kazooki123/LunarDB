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
    crow::response createJsonResponse(int status, const std::string& message,
                                    const crow::json::wvalue& data = {});
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
};

} // namespace api
} // namespace lunardb

#endif // API_HANDLER_H

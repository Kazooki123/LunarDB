#include "api_handler.h"
#include <string>
#include <vector>

namespace lunardb {
namespace api {

APIHandler::APIHandler(std::shared_ptr<Cache> cache) : cache_(cache) {
    setupRoutes();
}

void APIHandler::setupRoutes() {
    // Basic key-value operations
    CROW_ROUTE(app_, "/get/<string>").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req, std::string key){ return handleGet(req, key); });

    CROW_ROUTE(app_, "/set").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req){ return handleSet(req); });

    CROW_ROUTE(app_, "/delete/<string>").methods(crow::HTTPMethod::DELETE)
    ([this](std::string key){ return handleDelete(key); });

    CROW_ROUTE(app_, "/mget").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req){ return handleMget(req); });

    CROW_ROUTE(app_, "/mset").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req){ return handleMset(req); });

    CROW_ROUTE(app_, "/keys").methods(crow::HTTPMethod::GET)
    ([this](){ return handleKeys(); });

    // List operations
    CROW_ROUTE(app_, "/lpush").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req){ return handleLpush(req); });

    CROW_ROUTE(app_, "/rpush").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req){ return handleRpush(req); });

    CROW_ROUTE(app_, "/lpop/<string>").methods(crow::HTTPMethod::POST)
    ([this](std::string key){ return handleLpop(key); });

    CROW_ROUTE(app_, "/rpop/<string>").methods(crow::HTTPMethod::POST)
    ([this](std::string key){ return handleRpop(key); });

    CROW_ROUTE(app_, "/lrange").methods(crow::HTTPMethod::GET)
    ([this](const crow::request& req){ return handleLrange(req); });

    CROW_ROUTE(app_, "/llen/<string>").methods(crow::HTTPMethod::GET)
    ([this](std::string key){ return handleLlen(key); });
}

crow::response APIHandler::createJsonResponse(int status, const std::string& message,
                                           const crow::json::wvalue& data) {
    crow::json::wvalue response;
    response["status"] = status;
    response["message"] = message;
    if (!data.empty()) {
        response["data"] = data;
    }
    return crow::response(status, response);
}

bool APIHandler::validateKey(const std::string& key, crow::response& error) {
    if (key.empty()) {
        error = createJsonResponse(400, "Key cannot be empty");
        return false;
    }
    return true;
}

crow::response APIHandler::handleGet(const crow::request& req, const std::string& key) {
    crow::response error;
    if (!validateKey(key, error)) return error;

    std::string value = cache_->get(key);
    if (value.empty()) {
        return createJsonResponse(404, "Key not found");
    }

    crow::json::wvalue data;
    data["key"] = key;
    data["value"] = value;
    return createJsonResponse(200, "Success", data);
}

crow::response APIHandler::handleSet(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x) {
        return createJsonResponse(400, "Invalid JSON");
    }

    if (!x.has("key") || !x.has("value")) {
        return createJsonResponse(400, "Missing key or value");
    }

    std::string key = x["key"].s();
    std::string value = x["value"].s();
    int ttl = x.has("ttl") ? x["ttl"].i() : 0;

    crow::response error;
    if (!validateKey(key, error)) return error;

    cache_->set(key, value, ttl);
    return createJsonResponse(200, "Success");
}

crow::response APIHandler::handleDelete(const std::string& key) {
    crow::response error;
    if (!validateKey(key, error)) return error;

    if (cache_->del(key)) {
        return createJsonResponse(200, "Success");
    }
    return createJsonResponse(404, "Key not found");
}

crow::response APIHandler::handleMget(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x || !x.has("keys") || !x["keys"].is_list()) {
        return createJsonResponse(400, "Invalid request format");
    }

    std::vector<std::string> keys;
    for (const auto& key : x["keys"]) {
        keys.push_back(key.s());
    }

    auto values = cache_->mget(keys);

    crow::json::wvalue data;
    crow::json::wvalue::list result;
    for (size_t i = 0; i < keys.size(); i++) {
        crow::json::wvalue entry;
        entry["key"] = keys[i];
        entry["value"] = values[i];
        result.push_back(std::move(entry));
    }
    data["results"] = std::move(result);
    return createJsonResponse(200, "Success", data);
}

crow::response APIHandler::handleMset(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x || !x.has("pairs") || !x["pairs"].is_list()) {
        return createJsonResponse(400, "Invalid request format");
    }

    std::vector<std::pair<std::string, std::string>> pairs;
    for (const auto& pair : x["pairs"]) {
        if (!pair.has("key") || !pair.has("value")) {
            return createJsonResponse(400, "Invalid pair format");
        }
        pairs.emplace_back(pair["key"].s(), pair["value"].s());
    }

    cache_->mset(pairs);
    return createJsonResponse(200, "Success");
}

crow::response APIHandler::handleKeys() {
    auto keys = cache_->keys();
    crow::json::wvalue data;
    data["keys"] = std::move(keys);
    return createJsonResponse(200, "Success", data);
}

crow::response APIHandler::handleLpush(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x || !x.has("key") || !x.has("value")) {
        return createJsonResponse(400, "Invalid request format");
    }

    std::string key = x["key"].s();
    std::string value = x["value"].s();

    crow::response error;
    if (!validateKey(key, error)) return error;

    cache_->lpush(key, value);
    return createJsonResponse(200, "Success");
}

crow::response APIHandler::handleRpush(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x || !x.has("key") || !x.has("value")) {
        return createJsonResponse(400, "Invalid request format");
    }

    std::string key = x["key"].s();
    std::string value = x["value"].s();

    crow::response error;
    if (!validateKey(key, error)) return error;

    cache_->rpush(key, value);
    return createJsonResponse(200, "Success");
}

crow::response APIHandler::handleLpop(const std::string& key) {
    crow::response error;
    if (!validateKey(key, error)) return error;

    std::string value = cache_->lpop(key);
    if (value.empty()) {
        return createJsonResponse(404, "List empty or key not found");
    }

    crow::json::wvalue data;
    data["value"] = value;
    return createJsonResponse(200, "Success", data);
}

crow::response APIHandler::handleRpop(const std::string& key) {
    crow::response error;
    if (!validateKey(key, error)) return error;

    std::string value = cache_->rpop(key);
    if (value.empty()) {
        return createJsonResponse(404, "List empty or key not found");
    }

    crow::json::wvalue data;
    data["value"] = value;
    return createJsonResponse(200, "Success", data);
}

crow::response APIHandler::handleLrange(const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x || !x.has("key") || !x.has("start") || !x.has("stop")) {
        return createJsonResponse(400, "Invalid request format");
    }

    std::string key = x["key"].s();
    int start = x["start"].i();
    int stop = x["stop"].i();

    crow::response error;
    if (!validateKey(key, error)) return error;

    auto values = cache_->lrange(key, start, stop);

    crow::json::wvalue data;
    data["values"] = std::move(values);
    return createJsonResponse(200, "Success", data);
}

crow::response APIHandler::handleLlen(const std::string& key) {
    crow::response error;
    if (!validateKey(key, error)) return error;

    size_t length = cache_->llen(key);

    crow::json::wvalue data;
    data["length"] = static_cast<int>(length);
    return createJsonResponse(200, "Success", data);
}

void APIHandler::start(const std::string& host, uint16_t port) {
    app_.bindaddr(host).port(port).run();
}

void APIHandler::stop() {
    app_.stop();
}

} // namespace api
} // namespace lunardb

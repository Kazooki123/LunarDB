#pragma once

// Standard Library Includes
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <memory>
#include <limits>
#include <map>
#include <unordered_map>

// Lua Integration | UBUNTU
extern "C" {
    #include <lua5.4/lua.h>
    #include <lua5.4/lauxlib.h>
    #include <lua5.4/lualib.h>
}

// Project-specific includes
#include "cache.h"
#include "saved.h"
#include "sql.h"
#include "module.h"
#include "sharding.h"
#include "hashing.h"
#include "connect.h"
#include "concurrency.h"
#include "parser.h"
#include "providers/provider.hpp"
#include "providers/registry.hpp"

// Global variables declaration
extern Cache cache;
extern LunarDB::Connection connection;
extern TaskQueue taskQueue;
extern BackgroundProcessor bgProcessor;
extern LunarDB::ShardManager shardManager;
extern lua_State* L;

// Enums
enum class Mode {
    SCHEMAFULL,
    SCHEMALESS,
    SQL
};

// Function declarations
void printColoredText(const std::string& text, const std::string& color);
void printLunarLogo();
void printHelp();
void printSwitchOptions();
void printProviderHelp();
bool getProviderConfig(lunardb::providers::ProviderConfig& config);
void initializeLua();
void closeLua();
bool checkHealth();
bool executeLuaScript(const std::string& script);

// Template function
template<typename F, typename... Args>
double measureExecutionTime(F func, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

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

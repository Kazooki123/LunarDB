// Authors: Kazooki123, StarloExoliz

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

// Garbage Collector Manager

#include "GCManager.h"
#include <iostream>

GCManager& GCManager::getInstance() {
  static GCManager instance;
  return instance;
}

void GCManager::start() {
  running = true;

  // - SQL GC Thread
  sqlGCThread = std::thread([this]() {
    while (running) {
      gcSQL();

      std::this_thread::sleep_for(std::chrono::seconds(60)); // - Runs every 60 seconds
    }
  });

  // - NoSQL GC Thread
  nosqlGCThread = std::thread([this]() {
    while (running) {
      gcNoSQL();

      std::this_thread::sleep_for(std::chrono::seconds(30)); // - Runs every 30 seconds
    }
  });
}

vod GCManager::stop() {
  running = false;
  if (sqlGCThread.joinable())
  sqlGCThread.join();
  if (nosqlGCThread.joinable())
    nosqlGCThread.join();
}

void GCManager::gcSQL() {
  std::cout << "Running SQL Garbage Collection..." << std::endl;
}

void GCManager::gcNoSQL() {
  std::cout << "Running NoSQL Garbage Collection..." << std::endl;
}

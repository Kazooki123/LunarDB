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

// - MVCC = Multi Version Concurrency Control

#include "mvcc.h"
#include <algorithm>
#include <iostream>

// - Row Version Manager (RVM)

RowVersionManager::RowVersionManager(){}

void RowVersionManager::addVersion(int transaction_id, const std::string& data) {
  std::lock_guard<std::mutex> lock(version_mutex);
  int new_version_id = versions.empty() ? 1 : versions.back().version_id + 1;
  versions.push_back({new_version_id, data, false, transaction_id});
}

RowVersion RowVersionManager::getVersion(int transaction_id) const {
  std::lock_guard<std::mutex> lock(version_mutex);

  for (auto it = versions.rbegin(); it != versions.rend(); ++it) {
    if (it->is_committed || it->transaction_id == transaction_id) {
      return *it // - This returns the latest committed or current transaction VERSION
    }
  }

  throw std::runtime_error("No version available for this transaction!");
}

void RowVersionManager::cleanupOldVersions() {
  std::lock_guard<std::mutex> lock(version_mutex);

  // - Removes all non-committed or stale versions except the latest
  if (versions.size() > 1) {
    versions.erase(std::remove_if(versions.begin(), versions.end(), [](const RowVersion& v) { return !v.is_committed; }), versions.end());
  }
}

// - MVCCManager

MVCCManager::MVCCManager(){}

void MVCCManager::beginTransaction(int transaction_id) {
  std::lock_guard<std::mutex> lock(mvcc_mutex);
  active_transactions[transaction_id] = true;
}

void MVCCManager::endTransaction(int transaction_id) {
  std::lock_guard<std::mutex> lock(mvcc_mutex);
  active_transactions.erase(transaction_id);
}

void MVCCManager::addRowVersion(int row_id, int transaction_id, const std::string& data) {
  std::lock_guard<std::mutex> lock(mvcc_mutex);
  row_versions[row_id].addVersion(transaction_id, data);
}

std::string MVCCManager::getRowVersion(int row_id, int transaction_id) const {
  std::lock_guard<std::mutex> lock(mvcc_mutex);

  if (row_versions.find(row_id) != row_versions.end()) {
    return row_versions.at(row_id).getVersion(transaction_id).data;
  }
  throw std::runtime_error("Row not found..");
}

void MVCCManager::commitTransaction(int transaction_id) {
  std::lock_guard<std::mutex> lock(mvcc_mutex);

  for (auto& [row_id, row_manager] : row_versions) {
    manager.commitVersion(transaction_id);
  }
  endTransaction(transaction_id);
}

void MVCCManager::cleanup() {
  std::lock_guard<std::mutex> lock(mvcc_mutex);

  for (auto& [row_id, row_manager] : row_versions) {
    manager.cleanupOldVersions();
  }
}

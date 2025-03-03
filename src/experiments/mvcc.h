#ifndef MVCC_H
#define MVCC_H

#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>

struct RowVersion {
  int version_id;
  std::string data;
  bool is_committed;
  int transaction_id;
};

class RowVersionManager {
public:
  RowVersionManager();
  void addVersion(int transaction_id, const std::string &data);
  RowVersion getVersion(int transaction_id) const;
  void commitVersion(int transaction_id);
  void cleanupOldVersions();

private:
  std::vector<RowVersion> versions;
  mutable std::mutex version_mutex;
};

class MVCCManager {
public:
  MVCCManager();

  void beginTransaction(int transaction_id);
  void endTransaction(int row_id, int transaction_id, const std::string& data);
  std::string getRowVersion(int row_id, int transaction_id) const;
  void commitTransaction(int transaction_id);
  void cleanup();

private:
  std::unordered_map<int, RowVersionManager> row_versions;
  std::unordered_map<int, bool> active_transactions;
  mutable std::mutex mvcc_mutex; // - Mutex for thread safety
};

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

#endif // MVCC_H

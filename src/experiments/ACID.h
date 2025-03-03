#ifndef ACID_H
#define ACID_H

#include <string>
#include <vector>
#include <mutex>
#include <fstream>

class ACID {
private:
  // Atomicity
  std::vector<std::string> transactionLog;
  bool isCommitted;

  // Isolation
  std::mutex transactionMutex;

  // Durability
  const std::string walFile = "wal.log";

  void writeToWAL(const std::string& operation);
  void rollbackTransaction();

public:
  ACID();

  // Atomicity
  void beginTransaction();
  void logOperation(const std::string& operation);
  void commitTransaction();

  // Consistency
  bool validateOperation(const std::string& operation);

  // Isolation
  void acquireLock();
  void releaseLock();

  // Durability
  void recoverFromWAL();

  // Utility
  bool isTransactionCommitted() const;
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

#endif // ACID_H

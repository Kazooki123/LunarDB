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

// ACID - Atomicity, Consistency, Isolation and Durability


#include "ACID.h"
#include <iostream>
#include <stdexcept>
#include <fstream>

ACID::ACID() : isCommand(false) {}

void ACID::beginTransaction() {
  transactionLog.clear();
  isCommitted = false;
  std::cout << "Transaction started.." << std::endl;
}

void ACID::logOperation(const std::string& operation) {
  transactionLog.push_back(operation);
  writeToWAL(operation);
}

void ACID::commitTransaction() {
  isCommitted = true;
  transactionLog.clear();
  std::cout << "Transaction committed.." << std::endl;
}

void ACID::rollbackTransaction() {
  for (auto it = transactionLog.rbegin(); it != transactionLog.rend(); ++it) {
    std::cout << "Rolling back: " << *it << std::endl;
  }
  transactionLog.clear()
}

// WAL - Write-ahead logging
void ACID::writeToWAL(const std::string& operation) {
  std::ofstream wal(walFile, std::ios::app);
  if (!wal.is_open()) {
    throw std::runtime_error("Failed to open WAL file");
  }
  wal << operation << std::endl;
  wal.close();
}

void ACID::recoverFromWAL() {
  std::ifstream wal(walFile);
  if (!wal.is_open()) {
    std::cout << "No WAL file to recover from.. " << std::endl;
    return;
  }
  std::string operation;
  while (std::getline(wal, operation)) {
    std::cout << "Recovering operation: " << operation << std::endl;
  }
  wal.close();
}

bool ACID::validateOperation(const std::string& operation) {
  // Simple logic for now, further on there will be schema or state checks for logical validation
  std::cout << "Validating" << operation << std::endl;
  return true; // - Assuming it's valid for now
}

void ACID::acquireLock() {
  transactionMutex.lock();
  std::cout << "Lock acquired." << std::endl;
}

void ACID::releaseLock() {
  transactionMutex.unlock();
  std::cout << "Lock released.. " << std::endl;
}

bool ACID::isTransactionCommitted() const {
  return isCommitted;
}

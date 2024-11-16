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


#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include "connect.h"

namespace LunarDB {

Connection::Connection() : sock(-1), connected(false) {}

Connection::~Connection() {
    disconnect();
}

void Connection::connect(const std::string& ip, int port) {
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Convert IP address to binary form
    if (inet_pton(AF_INET, ip.c_str(), &server.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP address");
    }

    // Connect to server
    if (::connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        throw std::runtime_error("Connection failed");
    }

    connected = true;
}

void Connection::disconnect() {
    if (connected && sock != -1) {
        close(sock);
        sock = -1;
        connected = false;
    }
}

bool Connection::isConnected() const {
    return connected;
}

bool Connection::send(const std::string& message) {
    if (!connected) {
        return false;
    }
    return ::send(sock, message.c_str(), message.length(), 0) >= 0;
}

std::string Connection::receive(size_t buffer_size) {
    if (!connected) {
        return "";
    }

    char buffer[buffer_size];
    std::string received;

    ssize_t bytes = recv(sock, buffer, buffer_size - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        received = buffer;
    }

    return received;
}

} // namespace LunarDB

#include "server.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2 || std::string(argv[1]) != "serve") {
        std::cout << "Usage: lunardb serve [ip] [port]\n";
        return 1;
    }

    LunarDBServer server;

    std::string ip = "";
    int port = 0;

    if (argc >= 3) ip = argv[2];
    if (argc >= 4) port = std::stoi(argv[3]);

    server.serve(ip, port);

    return 0;
}

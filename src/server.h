#ifndef LUNARDB_SERVER_H
#define LUNARDB_SERVER_H

#include <string>

class LunarDBServer {
public:
    LunarDBServer();
    void serve(const std::string& ip, int port);
    
private:
    std::string getLocalIP() const;
    int findAvailablePort() const;
};

#endif // LUNARDB_SERVER_H
FROM alpine:latest

RUN apk add --no-cache g++ make lua5.4-dev libpq-dev wget cmake curl-dev boost-dev asio-dev

WORKDIR /usr/src/

# Install libpqxx from source
RUN wget https://github.com/jtv/libpqxx/archive/7.1.0.tar.gz && \
    tar -xf 7.1.0.tar.gz && \
    cd libpqxx-7.1.0 && \
    mkdir build && cd build && \
    cmake .. && \
    make && make install && \
    rm -rf /usr/src/libpqxx*

WORKDIR /usr/src/lunardb

COPY . .

RUN mkdir -p modules

RUN g++ -std=c++17 src/main.cpp src/core_MAIN.cpp src/cache.cpp src/connect.cpp src/concurrency.cpp src/saved.cpp src/sql.cpp src/module.cpp src/parser.cpp src/sharding.cpp src/hashing.cpp -I/usr/include/lua5.4 -lpqxx -lpq -lcurl -lboost_system -pthread -o bin/lunar $(pkg-config --libs lua5.4) && \
    chmod +x bin/lunardb

WORKDIR /usr/src/lunardb/bin

CMD ["./lunardb"]

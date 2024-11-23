FROM alpine:latest

RUN apk add --no-cache g++ make lua5.4-dev libpq-dev wget cmake curl-dev

WORKDIR /usr/src/

# Install libpqxx from source
RUN wget https://github.com/jtv/libpqxx/archive/7.6.0.tar.gz && \
    tar -xf 7.6.0.tar.gz && \
    cd libpqxx-7.6.0 && \
    mkdir build && cd build && \
    cmake .. && \
    make && make install && \
    rm -rf /usr/src/libpqxx*

WORKDIR /usr/src/lunardb

COPY . .

RUN mkdir -p modules

RUN g++ -std=c++17 src/main.cpp src/cache.cpp src/connect.cpp src/concurrency.cpp src/saved.cpp src/sql.cpp src/module.cpp src/hashing.cpp -I/usr/include/lua5.4 -lpqxx -lpq -lcurl -o bin/lunar $(pkg-config --libs lua5.4) && \
    chmod +x bin/lunar

WORKDIR /usr/src/lunardb/bin

CMD ["./lunar"]

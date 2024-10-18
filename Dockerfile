FROM alpine:latest

RUN apk add --no-cache g++ make lua5.4-dev

WORKDIR /usr/src/lunardb

COPY . .

RUN mkdir -p modules

RUN g++ -std=c++17 src/main.cpp src/cache.cpp src/saved.cpp src/sql.cpp src/module.cpp src/hashing.cpp -I/usr/include/lua5.4 -o bin/lunar $(pkg-config --libs lua5.4) && \
    chmod +x bin/lunar

WORKDIR /usr/src/lunardb/bin

CMD ["./lunar"]
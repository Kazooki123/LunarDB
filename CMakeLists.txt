cmake_minimum_required(VERSION 3.10)

project(lunardb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

include_directories(/usr/include/lua5.4)

set(SOURCES
  src/main.cpp
  src/core_MAIN.cpp
  src/cache.cpp
  src/connect.cpp
  src/concurrency.cpp
  src/saved.cpp
  src/sql.cpp
  src/module.cpp
  src/parser.cpp
  src/sharding.cpp
  src/hashing.cpp
)

add_executable(lunardb ${SOURCES})

target_link_libraries(lunardb lua5.4 pq curl boost_system pthread)

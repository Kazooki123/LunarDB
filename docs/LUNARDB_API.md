# LunarDB API System

A Documentation and Guide on how to use the [LunarDB API](htttps://github.com/Kazooki123/lunardb_api/)

First, the LunarDB API (Application Programming Interface) is made in **Go**
A memory safe and statically typed language great for API management.

I chose Go because of its simplicity and high level management for APIs
of LunarDB.

## How does it work?

Everything (e.g routes, handlers & keygen) are all packed in a single `main.go` file.

The file also **mimics** the functionality of LunarDB (SET, GET, LIST) since for now it is too
complex to call commands to a C++ script from a Go file.

## Commands to call the API

SET:

```bash
curl -X POST http://localhost:8080/set -d '{"key": "name", "value": "John"}'
```

GET:

```bash
curl http://localhost:8080/get -d '{"key": "name"}'
```

LIST:

```bash
curl http://localhost:8080/list
```

## How to run the API

```bash
go run main.go
```

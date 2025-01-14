# LunarDB API System (LATEST)

A guide of setting up and running the API server of **LunarDB**!

Note: The `Go` version is deprecated and will not be continued to develop.

## Notes

This feature is in `beta` and will changed over time.
Also: For Windows users, just note that you can't connect to the server port for whatever
reasons, so if you have a WSL Ubuntu(or others) installed, run it there. But before that, install WSL of course.

## Step 1

Run `api` from `bin/`

```bash
./api
```

## Step 2

Test it by running this:

```bash
curl -X POST http://0.0.0.0:18080/set -H "Content-Type: application/json" -d '{"key": "hello", "value": "world"}'
```

It should output something like this:

```bash
{"data":null,"message":"Success","status":200}
```

For `GET` and `KEYS`:

```bash
curl -X GET http://0.0.0.0:18080/get/hello
curl -X GET http://0.0.0.0:18080/keys
```

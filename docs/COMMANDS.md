# LunarDB Commands

A Guide to **how to use** LunarDBs commands and how they work:

## SET

Sets a key-value pair with an optional TTL (Time To Live) in seconds.

### Syntax

`SET key value [ttl]`

### Example

```bash
SET mykey "Hello, World!" 30
```

## GET

Gets the value of a key.

### Syntax for GET

`GET key`

### Example for GET

```bash
GET mykey
```

### Response

The response will be the value of the key, or `(nil)` if the key does not exist.

## DEL

Deletes a key-value pair.

### Syntax for DEL

`DEL key`

### Example for DEL

```bash
DEL mykey
```

## MSET

SET multiple key-value pairs.

### Syntax for MSET

`MSET key1 val1 key2 val2`

### Examples for MSET

```bash
> MSET hello world hows day
> OK
> MGET hello hows
world
day
```

## MGET

Gets multiple values.

### Syntax for MGET

`MGET key1 key2 ...`

### Example for MGET

```bash
MGET hello hows
```

## HASHING

LunarDB prioritizes safety first, we've added a hashing system and **COMMANDS** for you (the developer)
leverage your database to be secure as possible!

## HASH SHA256

Hashes a **KEY** with the **SHA-256** encryption method

### Syntax for HASH SHA256

`HASH SHA256 key`

### Example and output for HASH SHA256

```bash
HASH SHA256 hello
```

Output: `d6bb06a930160bf314cea9718eac86126be323c804c9f770db779e0773616108` (SHA256)

## HASH MURMUR3

Hashes a **KEY** with the **MURMUR3** encryption method

### Example and output for HASH MURMUR3

```bash
HASH MURMUR3 hello
```

Output: `3863771492` (MURMUR3)

## HASH ROTATE

Hashes/Shifts a **INPUT** base on how many shifts you've given (Checkout Caesar Encryption for details)

### Example of HASH ROTATE

```bash
HASH ROTATE hello 3
> khoor
```

hello - Input
3 - the number of shifts it's given

**For more information please visit the [official docs](https://lunardbdocs.vercel.app) website!**

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

**For more information please visit the [official docs](https://lunardbdocs.vercel.app) website!**

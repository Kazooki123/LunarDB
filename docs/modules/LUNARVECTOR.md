# 📖Guide: LunarVector Module

Here you can learn the basic function and commands for **LunarVector**!

## ❓What is it?

It is a vector database support module for LunarDB to extend its usability for developers out there,
all made in **Rust**, it is a fast vector database module that combines the speed with LunarDB as well.

## ⏰Basic Commands

- **VCREATE <db_name>**

Creates a database/key which you can used.

### Example for **`VCREATE`**

```bash
VCREATE mydb
```

- **VADD <db_key> <vector_id> "dim1" "dim2"**

Meaning Vector Add, it adds a vector to your database.

### Example for **`VADD`**

```bash
VADD mydb vec1 0.1 0.2 0.3 0.4 0.5
VADD mydb vec2 0.2 0.3 0.4 0.5 0.6
```

- **VGET <db_key> <vector_id>**

Gets the vector from your database.

### Example for **`VGET`**

```bash
VGET mydb vec1
```

For more information you can type **"HELP"** in the _command line_ when using the **LunarVector** module.

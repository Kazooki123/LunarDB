# Contributing to LunarDB

Thank you for your interest in contributing to LunarDB! We highly value your effort, time, and expertise in helping improve this project.

LunarDB is an open-source, cache-based, in-memory key-value store database, designed to be fast, efficient, and highly scalable. We welcome contributions from developers of all skill levels, whether you are fixing bugs, implementing new features, writing documentation, or helping with testing.

## Tech Stack

LunarDB utilizes a variety of programming languages and technologies, each serving a specific purpose:

- **C++** – The core functionality of LunarDB is implemented in C++ for performance optimization and memory management.
- **Lua** – Lua is used for scripting and extending the system, offering flexibility in configuring and automating tasks within LunarDB.
- **Rust** – Rust is employed for some of LunarDB's modules, ensuring memory safety and high concurrency without sacrificing performance.
- **Elixir** - Used for things like clustering, monitoring and fault tolerance1
- **Ruby** - For Unit and Integrated testing, now it's no longer for module handling.

## How to Contribute

We welcome a wide variety of contributions. Here's how you can get started:

### 1. Fork the Repository

Start by [forking the repository](https://github.com/Kazooki123/LunarDB/fork) to your own GitHub account.

### 2. Clone Your Fork

Clone your fork locally so you can begin making changes.

```bash
git clone https://github.com/Kazooki123/LunarDB.git
cd LunarDB
```

### 3. Create a New Branch

For each new feature, bug fix, or improvement, create a separate branch to keep your changes organized.

```bash
git checkout -b feature/my-new-feature
```

### 4. Commit Your Changes

Make sure to test your changes thoroughly and follow the project's coding standards. Once ready, commit your changes.

```bash
git add .
git commit -m "Description of the changes"
```

### 5. Push to Your Fork and Submit a Pull Request

Push your changes to your forked repository and submit a pull request to the main LunarDB repository.

```bash
git push origin feature/my-new-feature
```

### 6. Review Process

One of the project maintainers will review your pull request, provide feedback if necessary, and merge it once everything looks good. We aim to respond to pull requests within a few days.

### Additional Note

If you're recompiling lunar.exe make sure to run this command with [g++](https://gcc.gnu.org/):

### Important Note

**Note:** You would need:

- The Source codes for **Lua v5.4.4**
- For Linux this would be `liblua5.4-dev` (Ubuntu) and `lua-dev` (Arch)

Compiling for Windows:

**⚠️ LUNARDB IS CURRENTLY ON SUPPORT FOR LINUX FOR NOW SO COMPILING MIGHT BROKE THE MAIN WINDOWS EXE FILE AS `main.cpp` CONTAINS
LINUX-ONLY SYNTAXES FOR LINUX `g++` ⚠️**

```bash
g++ -std=c++17 main.cpp cache.cpp saved.cpp sql.cpp module.cpp hashing.cpp -o ../bin/lunar.exe
```

## Contribution Guidelines

- **Code Style**: Follow the existing code style for each language. We use linters and formatters to ensure consistency.
- **Tests**: Ensure all tests pass and write new tests if applicable. We highly encourage unit and integration tests for new features.
- **Documentation**: When adding new features, update the relevant documentation.
- **Commits**: Write meaningful commit messages and make small, focused commits.

## Issues and Discussions

If you encounter any bugs or have feature requests, please open an issue on our [GitHub issues page](https://github.com/Kazooki123/LunarDB/issues). We also welcome discussions on improvements and general feedback.

## Code of Conduct

We are committed to fostering a welcoming and inclusive environment. Please read our [Code of Conduct](CODE_OF_CONDUCT) to understand our expectations for behavior in the community.

Once again, thank you for contributing to LunarDB. We look forward to your contributions!

— The LunarDB Team

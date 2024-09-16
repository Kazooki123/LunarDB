#ifndef HASHING_H
#define HASHING_H

#include <string>
#include <cstdint>

namespace Hashing {

// MurmurHash3 implementation for string keys
uint32_t murmur3_32(const std::string& key, uint32_t seed = 0);

// SHA-256 implementation for data integrity
std::string sha256(const std::string& data);

// Bcrypt implementation for password hashing
std::string bcrypt_hash(const std::string& password, unsigned int work_factor = 10);
bool bcrypt_verify(const std::string& password, const std::string& hash);

// Consistent hashing (for potential future distributed caching)
uint32_t consistent_hash(const std::string& key, int num_nodes);

}

#endif // HASHING_H
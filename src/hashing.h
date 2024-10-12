#ifndef HASHING_H
#define HASHING_H

#include <string>
#include <vector>
#include <cstdint>

class Hashing {
public:
    static std::string sha256(const std::string& input);
    static uint32_t murmur3_32(const std::string& key, uint32_t seed = 0);
    static std::string rotateEncrypt(const std::string& input, int shift);
    static std::string rotateDecrypt(const std::string& input, int shift);

private:
    static const uint32_t sha256_k[];
    static const uint32_t sha256_h0[];

    static uint32_t rotr(uint32_t n, uint32_t d);
    static uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t ep0(uint32_t x);
    static uint32_t ep1(uint32_t x);
    static uint32_t sig0(uint32_t x);
    static uint32_t sig1(uint32_t x);
};

#endif // HASHING_H
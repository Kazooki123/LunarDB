/*
** Copyright 2024 Kazooki123
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the “Software”), to deal in the Software without restriction, including without
** limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
** of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
** conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial
** portions of the Software.
**
** THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
** LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
** THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**/


#include "hashing.h"
#include <cstring>
#include <sstream>
#include <iomanip>

const uint32_t Hashing::sha256_k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
};

const uint32_t Hashing::sha256_h0[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

uint32_t Hashing::rotr(uint32_t n, uint32_t d) {
    return (n >> d) | (n << (32 - d));
}

uint32_t Hashing::ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

uint32_t Hashing::maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t Hashing::ep0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

uint32_t Hashing::ep1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

uint32_t Hashing::sig0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint32_t Hashing::sig1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

std::string Hashing::sha256(const std::string& input) {
    std::vector<uint8_t> message;
    message.reserve(input.size());
    for (char c : input) {
        message.push_back(static_cast<uint8_t>(c));
    }

    uint64_t message_length = message.size() * 8;
    message.push_back(0x80);
    while ((message.size() * 8 + 64) % 512 != 0) {
        message.push_back(0x00);
    }
    for (int i = 7; i >= 0; --i) {
        message.push_back(static_cast<uint8_t>((message_length >> (i * 8)) & 0xFF));
    }

    std::vector<uint32_t> w(64);
    uint32_t state[8];
    std::memcpy(state, sha256_h0, 32);

    for (size_t chunk = 0; chunk * 64 < message.size(); ++chunk) {
        for (int i = 0; i < 16; ++i) {
            w[i] = (message[chunk * 64 + i * 4] << 24) |
                   (message[chunk * 64 + i * 4 + 1] << 16) |
                   (message[chunk * 64 + i * 4 + 2] << 8) |
                   (message[chunk * 64 + i * 4 + 3]);
        }

        for (int i = 16; i < 64; ++i) {
            w[i] = sig1(w[i - 2]) + w[i - 7] + sig0(w[i - 15]) + w[i - 16];
        }

        uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
        uint32_t e = state[4], f = state[5], g = state[6], h = state[7];

        for (int i = 0; i < 64; ++i) {
            uint32_t t1 = h + ep1(e) + ch(e, f, g) + sha256_k[i] + w[i];
            uint32_t t2 = ep0(a) + maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
        state[4] += e; state[5] += f; state[6] += g; state[7] += h;
    }

    std::stringstream ss;
    for (int i = 0; i < 8; ++i) {
        ss << std::hex << std::setw(8) << std::setfill('0') << state[i];
    }
    return ss.str();
}

uint32_t Hashing::murmur3_32(const std::string& key, uint32_t seed) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(key.c_str());
    int len = static_cast<int>(key.length());
    const int nblocks = len / 4;

    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    const uint32_t* blocks = reinterpret_cast<const uint32_t*>(data + nblocks * 4);

    for (int i = -nblocks; i; ++i) {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = rotr(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotr(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    const uint8_t* tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1; k1 = rotr(k1, 15); k1 *= c2; h1 ^= k1;
    }

    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

std::string Hashing::rotateEncrypt(const std::string& input, int shift) {
    std::string result = input;
    for (char& c : result) {
        if (std::isalpha(c)) {
            char base = std::isupper(c) ? 'A' : 'a';
            c = static_cast<char>(((c - base + shift) % 26 + 26) % 26 + base);
        }
    }
    return result;
}

std::string Hashing::rotateDecrypt(const std::string& input, int shift) {
    return rotateEncrypt(input, -shift);
}

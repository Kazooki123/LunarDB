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

#endif // HASHING_H
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_map>

class Tokenizer {
public:
    enum class TokenType {
        SELECT, INSERT, UPDATE, MATCH, WHERE, RETURN, FROM, INTO, IDENTIFIER, STRING, NUMBER, SYMBOL, END
    };

    struct Token {
        TokenType type;
        std::string value;
    };

    explicit Tokenizer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t position;
    char currentChar();

    void advance();
    void skipWhitespace();
    void skipComment();
    Token identifier();
    Token number();
    Token string();
    Token symbol();
};

class Parser {
public:
    explicit Parser(const std::vector<Tokenizer::Token>& tokens);
    void parse();

private:
    std::vector<Tokenizer::Token> tokens;
    size_t currentPosition;

    void parseStatement();
    void parseSelect();
    void parseInsert();
    void parseMatch();
    void parseUpdate();
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

#endif // PARSER_H

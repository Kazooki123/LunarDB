// Authors: Kazooki123, Starloexoliz

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

/*
**
** In this file it executes the '.lrql' file or the LunarDB Query Language (custom)
** the CLI then executes the file but it finds a TABLE first or else it throws an error
**
**/


#include "parser.h"
#include <cctype>
#include <iostream>

Tokenizer::Tokenizer(const std::string& source) : source(source), position(0) {}

char Tokenizer::currentChar() {
    return position < source.size() ? source[position] : '\0';
}

void Tokenizer::advance() {
    position++;
}

void Tokenizer::skipWhitespace() {
    while (std::isspace(currentChar())) {
        advance();
    }
}

void Tokenizer::skipComment() {
    if (currentChar() == '#') {
        while (currentChar() != '\n' && currentChar() != '\0') {
            advance();
        }
    }
}

Tokenizer::Token Tokenizer::identifier() {
    std::string value;
    while (std::isalnum(currentChar()) || currentChar() == '_') {
        value += currentChar();
        advance();
    }
    // Map keywords to token types
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"SELECT", TokenType::SELECT}, {"INSERT", TokenType::INSERT},
        {"MATCH", TokenType::MATCH}, {"WHERE", TokenType::WHERE},
        {"RETURN", TokenType::RETURN}, {"FROM", TokenType::FROM},
        {"INTO", TokenType::INTO}, {"UPDATE", TokenType::UPDATE}
    };
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return {it->second, value};
    }
    return {TokenType::IDENTIFIER, value};
}

Tokenizer::Token Tokenizer::number() {
    std::string value;
    while (std::isdigit(currentChar())) {
        value += currentChar();
        advance();
    }
    return {TokenType::NUMBER, value};
}

Tokenizer::Token Tokenizer::string() {
    std::string value;
    advance();
    while (currentChar() != '\'' && currentChar() != '\0') {
        value += currentChar();
        advance();
    }
    advance();
    return {TokenType::STRING, value};
}

Tokenizer::Token Tokenizer::symbol() {
    char sym = currentChar();
    advance();
    return {TokenType::SYMBOL, std::string(1, sym)};
}

std::vector<Tokenizer::Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    while (currentChar() != '\0') {
        skipWhitespace();
        skipComment();
        if (currentChar() == '\0') break;
        if (std::isalpha(currentChar())) {
            tokens.push_back(identifier());
        } else if (std::isdigit(currentChar())) {
            tokens.push_back(number());
        } else if (currentChar() == '\'') {
            tokens.push_back(string());
        } else {
            tokens.push_back(symbol());
        }
    }
    tokens.push_back({TokenType::END, ""});
    return tokens;
}

Parser::Parser(const std::vector<Tokenizer::Token>& tokens) : tokens(tokens), currentPosition(0) {}

void Parser::parse() {
    while (currentPosition < tokens.size() && tokens[currentPosition].type != Tokenizer::TokenType::END) {
        parseStatement();
    }
}

void Parser::parseStatement() {
    switch (tokens[currentPosition].type) {
        case Tokenizer::TokenType::SELECT:
            parseSelect();
            break;
        case Tokenizer::TokenType::INSERT:
            parseInsert();
            break;
        case Tokenizer::TokenType::MATCH:
            parseMatch();
            break;
        case Tokenizer::TokenType::UPDATE:
            parseUpdate();
            break;
        default:
            std::cerr << "Unexpected token: " << tokens[currentPosition].value << std::endl;
            currentPosition++;
            break;
    }
}

void Parser::parseSelect() {
    std::cout << "Parsing SELECT statement\n";
    currentPosition++;
}

void Parser::parseInsert() {
    std::cout << "Parsing INSERT statement\n";
    currentPosition++;
}

void Parser::parseMatch() {
    std::cout << "Parsing MATCH statement\n";
    currentPosition++;
}

void Parser::parseUpdate() {
    std::cout << "Parsing UPDATE statement \n";
    currentPosition++;
}

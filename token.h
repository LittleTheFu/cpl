#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>

enum class TokenType
{
    IF,
    ELSE,
    WHILE,
    INT,
    RETURN,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,

    ASSIGN,
    EQUAL,
    NOT_EQUAL,

    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    IDENTIFIER,
    INTEGER,

    EOF_TOKEN,

    UNKNOWN,
};

std::string tokenTypeToString(TokenType type);

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token()
    {
        type = TokenType::UNKNOWN;
        lexeme = "";
        line = 0;
        column = 0;
    }

    std::string toString()
    {
        return tokenTypeToString(type) + " " + lexeme + " " + std::to_string(line) + " " + std::to_string(column);
    }
};

#endif // _TOKEN_H_
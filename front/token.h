#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>

enum class _TokenType_
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

    L_PAREN,
    R_PAREN,
    L_BRACE,
    R_BRACE,
    SEMICOLON,
    COMMA,

    IDENTIFIER,
    INTEGER,

    EOF_TOKEN,

    UNKNOWN,
};

std::string tokenTypeToString(_TokenType_ type);

struct Token
{
    _TokenType_ type;
    std::string lexeme;
    int line;
    int column;

    Token()
    {
        type = _TokenType_::UNKNOWN;
        lexeme = "";
        line = 0;
        column = 0;
    }

    Token(_TokenType_ type, std::string lexeme)
    {
        this->type = type;
        this->lexeme = lexeme;
        this->line = 0;
        this->column = 0;
    }

    std::string toString()
    {
        return tokenTypeToString(type) + " " + lexeme + " " + std::to_string(line) + " " + std::to_string(column);
    }
};

#endif // _TOKEN_H_
#include "token.h"

std::string tokenTypeToString(_TokenType_ type)
{
    switch (type)
    {
    case _TokenType_::IF:
        return "IF";
    case _TokenType_::ELSE:
        return "ELSE";
    case _TokenType_::WHILE:
        return "WHILE";
    case _TokenType_::INT:
        return "INT";
    case _TokenType_::RETURN:
        return "RETURN";
    case _TokenType_::PLUS:
        return "PLUS";
    case _TokenType_::MINUS:
        return "MINUS";
    case _TokenType_::MULTIPLY:
        return "MULTIPLY";
    case _TokenType_::DIVIDE:
        return "DIVIDE";
    case _TokenType_::ASSIGN:
        return "ASSIGN";
    case _TokenType_::EQUAL:
        return "EQUAL";
    case _TokenType_::NOT_EQUAL:
        return "NOT_EQUAL";
    case _TokenType_::LESS:
        return "LESS";
    case _TokenType_::LESS_EQUAL:
        return "LESS_EQUAL";
    case _TokenType_::GREATER:
        return "GREATER";
    case _TokenType_::GREATER_EQUAL:
        return "GREATER_EQUAL";
    case _TokenType_::IDENTIFIER:
        return "IDENTIFIER";
    case _TokenType_::INTEGER:
        return "INTEGER";
    case _TokenType_::EOF_TOKEN:
        return "EOF_TOKEN";
    case _TokenType_::L_PAREN:
        return "L_PAREN";
    case _TokenType_::R_PAREN:
        return "R_PAREN";
    case _TokenType_::L_BRACE:
        return "L_BRACE";
    case _TokenType_::R_BRACE:
        return "R_BRACE";
    case _TokenType_::SEMICOLON:
        return "SEMICOLON";
    case _TokenType_::COMMA:
        return "COMMA";
    case _TokenType_::UNKNOWN:
        return "UNKNOWN";
    default:
        return "UNKNOWN";
    }

    return "";
}
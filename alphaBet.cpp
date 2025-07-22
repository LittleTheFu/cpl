#include "alphaBet.h"

/*
 * 32:   (space)
 * 33: ! (exclamation mark)
 * 34: " (double quote)
 * 35: # (hash/pound sign)
 * 36: $ (dollar sign)
 * 37: % (percent sign)
 * 38: & (ampersand)
 * 39: ' (single quote/apostrophe)
 * 40: ( (left parenthesis)
 * 41: ) (right parenthesis)
 * 42: * (asterisk)
 * 43: + (plus sign)
 * 44: , (comma)
 * 45: - (hyphen/minus sign)
 * 46: . (period/dot)
 * 47: / (slash)
 * 48-57: 0-9 (digits)
 * 58: : (colon)
 * 59: ; (semicolon)
 * 60: < (less than sign)
 * 61: = (equals sign)
 * 62: > (greater than sign)
 * 63: ? (question mark)
 * 64: @ (at sign)
 * 65-90: A-Z (uppercase letters)
 * 91: [ (left square bracket)
 * 92: \ (backslash)
 * 93: ] (right square bracket)
 * 94: ^ (caret/circumflex)
 * 95: _ (underscore)
 * 96: ` (backtick/grave accent)
 * 97-122: a-z (lowercase letters)
 * 123: { (left curly brace)
 * 124: | (vertical bar/pipe)
 * 125: } (right curly brace)
 * 126: ~ (tilde)
 */

AlphaBet::AlphaBet()
{
    for (int i = 32; i <= 126; ++i)
    {
        charactars_.insert(static_cast<char>(i));
    }

    charactars_.insert('\t'); // Tab
    charactars_.insert('\n'); // Line Feed / Newline
    charactars_.insert('\r'); // Carriage Return
}

AlphaBet &AlphaBet::instance()
{
    static AlphaBet instance;
    return instance;
}

const std::set<char> &AlphaBet::getcharactars() const
{
    return charactars_;
}

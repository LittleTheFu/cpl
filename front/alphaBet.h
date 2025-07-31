#ifndef _ALPHA_BET_H_
#define _ALPHA_BET_H_

#include <set>

class AlphaBet
{
public:
    AlphaBet();
    ~AlphaBet() = default;

    AlphaBet(const AlphaBet&) = delete;
    AlphaBet& operator=(const AlphaBet&) = delete;

    static AlphaBet& instance();

    const std::set<char>& getcharactars() const;
    bool isInAlphaBet(char c) const;

    const std::set<char>& getWhiteSpaceChar() const;
    const std::set<char>& getDigitChar() const;
    const std::set<char>& getWordChar() const;

private:
    std::set<char> charactars_;

    std::set<char> whiteSpaceChar_;
    std::set<char> digitChar_;
    std::set<char> wordChar_;
};

#endif
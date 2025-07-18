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

    const std::set<char>& getCharactors() const;

private:
    std::set<char> charactors_;
};

#endif
#ifndef _REGEX_H_
#define _REGEX_H_

#include <memory>
#include <string>
#include <map>
#include <set>
#include "regExNode.h"
#include "dfaState.h"
#include "nfaState.h"

class RegEx
{
public:
    // explicit RegEx(const std::string& regex);
    explicit RegEx(std::shared_ptr<RegExNode> root);
    ~RegEx();

    bool match(const std::string& str);

private:
    void buildDFA();
    std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr> epsilonClosure(const std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr>& states);
    bool isContainEndState(const std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr>& states);

private:
    // std::string regex_;
    std::shared_ptr<RegExNode> root_;
    std::shared_ptr<NfaStateFragment> nfaStateFragment_;
    std::map<std::set<std::shared_ptr<NFAState>, CompareNfaStateSharedPtr>, std::shared_ptr<DFAState>> dfaStateMap_;

    std::shared_ptr<DFAState> startDFAState_;
    std::shared_ptr<DFAState> endDFAState_;
};

#endif // _REGEX_H_
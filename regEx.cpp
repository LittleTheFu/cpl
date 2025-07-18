#include "regEx.h"

// RegEx::RegEx(const std::string &regex)
// {
//     regex_ = regex;
// }

RegEx::RegEx(std::shared_ptr<RegExNode> root)
{
    root_ = root;

    nfaStateFragment_ = root_->buildNfaStateFragment();
}

RegEx::~RegEx()
{
}

void RegEx::buildDFA()
{
}
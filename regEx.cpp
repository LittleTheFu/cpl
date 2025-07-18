#include "regEx.h"

// RegEx::RegEx(const std::string &regex)
// {
//     regex_ = regex;
// }

RegEx::RegEx(std::shared_ptr<RegExNode> root)
{
    root_ = root;
}

RegEx::~RegEx()
{
}

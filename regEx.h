#ifndef _REGEX_H_
#define _REGEX_H_

#include <memory>
#include <string>
#include "regExNode.h"

class RegEx
{
public:
    // explicit RegEx(const std::string& regex);
    explicit RegEx(std::shared_ptr<RegExNode> root);
    ~RegEx();

private:
    // std::string regex_;
    std::shared_ptr<RegExNode> root_;
};

#endif // _REGEX_H_
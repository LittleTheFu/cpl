#ifndef _WORK_SHOP_H_
#define _WORK_SHOP_H_

#include "virtualMachine.h"

class WorkShop
{
public:
    WorkShop();
    ~WorkShop() = default;

    WorkShop(const WorkShop&) = delete;
    WorkShop& operator=(const WorkShop&) = delete;
    WorkShop(WorkShop&&) = delete;
    WorkShop& operator=(WorkShop&&) = delete;

    static WorkShop& getInstance();

    std::vector<std::string> getVMSrc();

private:
    VirtualMachine vm_;
};

#endif // _WORK_SHOP_H_
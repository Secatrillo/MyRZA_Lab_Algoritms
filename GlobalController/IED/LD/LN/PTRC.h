#pragma once

#include "CDC/ACT.h"
#include "GenLogicalNodeClass.h"

class PTRC : public GenLogicalNodeClass
{
public:
    std::shared_ptr<ACT> Tr;

    PTRC(std::string LogicalNodeName_ = NULL, std::string LogicalDeviceRef_ = NULL);
    void formTrip(const std::vector<std::shared_ptr<ACT>> opList);
};

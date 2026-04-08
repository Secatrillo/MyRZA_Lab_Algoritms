#pragma once

#include "GenLogicalNodeClass.h"
#include "CDC/WYE.h"
#include "CDC/SAV.h"


class MMXU : public GenLogicalNodeClass
{
public:
    std::shared_ptr<WYE> A;
    std::shared_ptr<SAV> currentA;
    std::shared_ptr<SAV> currentB;
    std::shared_ptr<SAV> currentC;

    MMXU(std::string LogicalNodeName_ = NULL, std::string LogicalDeviceRef_ = NULL);
};


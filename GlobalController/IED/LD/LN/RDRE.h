#pragma once

#include "CDC/SPS.h"
#include "CDC/INS.h"
#include "GenLogicalNodeClass.h"

class RDRE : public GenLogicalNodeClass
{
public:
    std::shared_ptr<SPS> RcdMade;
    std::shared_ptr<INS> FltNum;
    int RcdChNum = 0;
    int RcdAChNum = 0;
    int RcdDChNum = 0;

    RDRE(std::string LogicalNodeName_, std::string LogicalDeviceRef_);
};


#pragma once

#include "CDC/SPS.h"
#include "CDC/INS.h"
#include "CDC/DPC.h"
#include "CDC/SPC.h"
#include "CDC/ACT.h"
#include "GenLogicalNodeClass.h"


class XCBR : public GenLogicalNodeClass
{
public:
    std::shared_ptr<SPS> Loc;
    std::shared_ptr<INS> OpCnt;
    std::shared_ptr<DPC> Pos;
    std::shared_ptr<SPC> BlckOpn;
    std::shared_ptr<SPC> BlckCls;

    XCBR(std::string LogicalNodeName_ = NULL, std::string LogicalDeviceRef_ = NULL);
    void actOnOpen(std::shared_ptr<ACT> Op);
};


#ifndef XCBR_H
#define XCBR_H

#include "CDC/SPS.h"
#include "CDC/INS.h"
#include "CDC/DPC.h"
#include "CDC/SPC.h"
#include "CDC/ACT.h"
#include "GenLogicalNodeClass.h"
#include "../../../../include.h"

class XCBR : public GenLogicalNodeClass
{
public:
    SPS Loc;
    INS OpCnt;
    DPC Pos;
    SPC BlckOpn;
    SPC BlckCls;

    XCBR(string LogicalNodeName_ = NULL, string LogicalDeviceRef_ = NULL);
    void actOnOpen(ACT Op);
};

#endif
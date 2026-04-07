#ifndef RDRE_H
#define RDRE_H

#include "CDC/SPS.h"
#include "CDC/INS.h"
#include "GenLogicalNodeClass.h"
#include "../../../../include.h"

class RDRE : public GenLogicalNodeClass
{
public:
    SPS RcdMade;
    INS FltNum;
    int RcdChNum = 0;
    int RcdAChNum = 0;
    int RcdDChNum = 0;

    RDRE(string LogicalNodeName_, string LogicalDeviceRef_);
};

#endif
#ifndef MMXU_H
#define MMXU_H

#include "GenLogicalNodeClass.h"
#include "CDC/WYE.h"
#include "CDC/SAV.h"
#include "../../../../include.h"

class MMXU : public GenLogicalNodeClass
{
public:
    WYE A;
    SAV currentA;
    SAV currentB;
    SAV currentC;

    MMXU(string LogicalNodeName_ = NULL, string LogicalDeviceRef_ = NULL);
};


#endif
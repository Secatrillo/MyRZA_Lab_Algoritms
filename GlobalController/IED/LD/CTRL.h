#ifndef CTRL_H
#define CTRL_H

#include "LN/CDC/ACT.h"
#include "GenLogicalDeviceClass.h"
#include "LN/CSWI.h"
#include "LN/XCBR.h"
#include "../../../include.h"

class CTRL : public GenLogicalDeviceClass
{
public:
    CSWI CSWI1;
    XCBR XCBR1;
    CTRL(string LDName_);
    void receiveTripSignal(ACT TripSignal);
};

#endif
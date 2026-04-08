#pragma once

#include "LN/CDC/ACT.h"
#include "GenLogicalDeviceClass.h"
#include "LN/CSWI.h"
#include "LN/XCBR.h"


class CTRL : public GenLogicalDeviceClass
{
public:
    CSWI CSWI1;
    XCBR XCBR1;
    CTRL(std::string LDName_);
    void receiveTripSignal(std::shared_ptr<ACT> TripSignal);
};


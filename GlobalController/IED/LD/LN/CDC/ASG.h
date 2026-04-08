#pragma once 

#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "GenCommonDataClass.h"
#include "CDT/AnalogValue.h"
#include "GenDataObjectClass.h"
#include <string>

class ASG : public GenCommonDataClass, public GenDataObjectClass
{
public:
    ptr<AnalogValue> setMag;

    ASG(std::string DataObjectName_, std::string LNRef_, bool Presence_);
};


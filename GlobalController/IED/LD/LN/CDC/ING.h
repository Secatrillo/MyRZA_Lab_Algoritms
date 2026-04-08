#pragma once

#include "CDT/GenDataAttributeClass.h"
#include "GenCommonDataClass.h"
#include "GenDataObjectClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include <string>

class ING : public GenCommonDataClass, public GenDataObjectClass
{
public:
    ptr<GenDataAttributeClass> setVal;

    ING(std::string DataObjectName_, std::string LNRef_, bool Presence_);
};


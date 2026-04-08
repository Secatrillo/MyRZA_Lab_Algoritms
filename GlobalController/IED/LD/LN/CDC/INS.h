#pragma once

#include "CDT/GenDataAttributeClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "GenCommonDataClass.h"
#include "CDT/Quality.h"
#include "CDT/TimeStamp.h"
#include "GenDataObjectClass.h"
#include <string>

class INS : public GenCommonDataClass, public GenDataObjectClass
{
public:
    ptr<GenDataAttributeClass> stVal;
    ptr<Quality> q;
    ptr<TimeStamp> t;
    INS(std::string DataObjectName_, std::string LNRef_, bool Presence_);
};


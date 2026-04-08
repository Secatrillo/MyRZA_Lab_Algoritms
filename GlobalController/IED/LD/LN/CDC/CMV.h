#pragma once

#include "GenCommonDataClass.h"
#include "GenDataObjectClass.h"
#include "CDT/Vector.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/Quality.h"
#include "CDT/TriggerOption.h"
#include "CDT/TimeStamp.h"
#include <string>

class CMV : public GenCommonDataClass, public GenDataObjectClass
{
public:
    ptr<Vector> cVal;
    ptr<Quality> q;
    ptr<TimeStamp> t;
    CMV(std::string SubDataObjectName_, bool Presence_, std::string DataObjectRef_);
};


#pragma once

#include "GenCommonDataClass.h"
#include "GenDataObjectClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/Quality.h"
#include "CDT/TriggerOption.h"
#include "CDT/AnalogValue.h"
#include <string>

class SAV: public GenCommonDataClass, public GenDataObjectClass
{

public:
    ptr<AnalogValue> instMag;
    ptr<Quality> q;

    SAV(std::string DataObjectName_, std::string LogicNodeRef_, bool Presence_);
    void setInstMag(double newInstMag);
    double getInstMag();
};


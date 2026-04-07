#ifndef SAV_H
#define SAV_H

#include "GenCommonDataClass.h"
#include "GenDataObjectClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/Quality.h"
#include "CDT/TriggerOption.h"
#include "CDT/AnalogValue.h"
#include "../../../../../include.h"

class SAV: public GenCommonDataClass, public GenDataObjectClass
{

public:
    AnalogValue instMag;
    Quality q;

    SAV(string DataObjectName_, string LogicNodeRef_, bool Presence_);
    void setInstMag(double newInstMag);
    double getInstMag();
};

#endif
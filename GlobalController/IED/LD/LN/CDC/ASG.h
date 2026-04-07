#ifndef ASG_H
#define ASG_H

#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "GenCommonDataClass.h"
#include "CDT/AnalogValue.h"
#include "GenDataObjectClass.h"
#include "../../../../../include.h"

class ASG : public GenCommonDataClass, public GenDataObjectClass
{
public:
    AnalogValue setMag;

    ASG(string DataObjectName_, string LNRef_, bool Presence_);
};

#endif
#ifndef SPS_H
#define SPS_H

#include "GenDataAttributeClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "GenCommonDataClass.h"
#include "CDT/Quality.h"
#include "CDT/TimeStamp.h"
#include "GenDataObjectClass.h"
#include "../../../../../include.h"

class SPS : public GenCommonDataClass, public GenDataObjectClass
{
public:
    GenDataAttributeClass stVal;
    Quality q;
    TimeStamp t;

    SPS(string DataObjectName_, string LNRef_, bool Presence_);
};

#endif
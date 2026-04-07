#ifndef DPC_H
#define DPC_H

#include "GenDataAttributeClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "GenCommonDataClass.h"
#include "CDT/Quality.h"
#include "CDT/TimeStamp.h"
#include "GenDataObjectClass.h"
#include "../../../../../include.h"

class DPC : public GenCommonDataClass, public GenDataObjectClass
{
public:
    GenDataAttributeClass stVal;
    Quality q;
    TimeStamp t;
    GenDataAttributeClass ctlModel;
    DPC(string DataObjectName_, string LNRef_, bool Presence_);
};

#endif
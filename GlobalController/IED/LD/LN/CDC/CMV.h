#ifndef CMV_H
#define CMV_H

#include "GenCommonDataClass.h"
#include "GenDataObjectClass.h"
#include "CDT/Vector.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/Quality.h"
#include "CDT/TriggerOption.h"
#include "CDT/TimeStamp.h"
#include "../../../../../include.h"

class CMV : public GenCommonDataClass, public GenDataObjectClass
{
public:
    Vector cVal;
    Quality q;
    TimeStamp t;
    CMV(string SubDataObjectName_, bool Presence_, string DataObjectRef_);
};


#endif
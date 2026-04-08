#pragma once

#include "CDT/GenDataAttributeClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "GenCommonDataClass.h"
#include "CDT/Quality.h"
#include "CDT/TimeStamp.h"
#include "GenDataObjectClass.h"
#include <string>


class ACT : public GenCommonDataClass, public GenDataObjectClass
{
public:
    ptr<GDAClass> general;
    ptr<Quality> q;
    ptr<TimeStamp> t;
    ptr<GDAClass> phsA;
    ptr<GDAClass> phsB;
    ptr<GDAClass> phsC;

    ACT(std::string DataObjectName_, std::string LNRef_, bool Presence_);
};


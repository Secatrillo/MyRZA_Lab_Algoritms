#pragma once

#include "CDT/GenDataAttributeClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "GenCommonDataClass.h"
#include "CDT/Quality.h"
#include "CDT/TimeStamp.h"
#include "GenDataObjectClass.h"


class ACD : public GenCommonDataClass, public GenDataObjectClass
{
public:
    ptr<GDAClass> general;  //срабатывание наравленной защиты общее
    ptr<GDAClass> dirGeneral;  //направленность защиты общее
    ptr<Quality> q;  //метка качества
    ptr<TimeStamp> t;  //метка времени
    ptr<GDAClass> phsA;  //срабатывание защиты в фазе А
    ptr<GDAClass> phsB;  //срабатывание защиты в фазе В
    ptr<GDAClass> phsC;  //срабатывание защиты в фазе С

    ACD(std::string DataObjectName_, std::string LNRef_, bool Presence_);
};


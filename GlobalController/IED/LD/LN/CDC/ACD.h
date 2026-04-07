#ifndef ACD_H
#define ACD_H

#include "GenDataAttributeClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "GenCommonDataClass.h"
#include "CDT/Quality.h"
#include "CDT/TimeStamp.h"
#include "GenDataObjectClass.h"
#include "../../../../../include.h"

class ACD : public GenCommonDataClass, public GenDataObjectClass
{
public:
    GenDataAttributeClass general;  //срабатывание наравленной защиты общее
    GenDataAttributeClass dirGeneral;  //направленность защиты общее
    Quality q;  //метка качества
    TimeStamp t;  //метка времени
    GenDataAttributeClass phsA;  //срабатывание защиты в фазе А
    GenDataAttributeClass phsB;  //срабатывание защиты в фазе В
    GenDataAttributeClass phsC;  //срабатывание защиты в фазе С

    ACD(string DataObjectName_, string LNRef_, bool Presence_);
};

#endif
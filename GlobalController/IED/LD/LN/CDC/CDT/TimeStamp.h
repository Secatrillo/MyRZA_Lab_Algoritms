#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "TriggerOption.h"
#include "EnumFunctionalConstraints.h"
#include "TimeQuality.h"
#include "../GenDataAttributeClass.h"
#include "../../../../../../include.h"

class TimeStamp : public GenDataAttributeClass
{

public:
    int SecondSinceEpoch;
    int FractionOfSecond;
    TimeQuality TimeQuality;
    TimeStamp(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, string DataObjectRef_);
};

#endif
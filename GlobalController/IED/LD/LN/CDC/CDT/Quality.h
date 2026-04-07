#ifndef QUALITY_H
#define QUALITY_H

#include "../GenDataAttributeClass.h"
#include "TriggerOption.h"
#include "EnumFunctionalConstraints.h"
#include "ValidityEnum.h"
#include "SourceEnum.h"
#include "../../../../../../include.h"

class Quality: public GenDataAttributeClass
{
public:
    ValidityEnum validity;
    bool overflow;
    bool outOfRange;
    bool badReference;
    bool oscillatory;
    bool failure;
    bool oldData;
    bool inconsistent;
    bool inaccurate;
    SourceEnum source;
    bool test;
    bool operatorBlocked;

    Quality(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, string DataObjectRef_);
};

#endif
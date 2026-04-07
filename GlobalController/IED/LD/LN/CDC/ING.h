#ifndef ING_H
#define ING_H

#include "GenDataAttributeClass.h"
#include "GenCommonDataClass.h"
#include "GenDataObjectClass.h"
#include "CDT/EnumFunctionalConstraints.h"
#include "CDT/TriggerOption.h"
#include "../../../../../include.h"

class ING : public GenCommonDataClass, public GenDataObjectClass
{
public:
    GenDataAttributeClass setVal;

    ING(string DataObjectName_, string LNRef_, bool Presence_);
};

#endif
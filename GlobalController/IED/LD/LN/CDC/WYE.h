#ifndef WYE_H
#define WYE_H

#include "GenCommonDataClass.h"
#include "GenDataObjectClass.h"
#include "CMV.h"
#include "../../../../../include.h"

class WYE : public GenCommonDataClass, public GenDataObjectClass
{
public:
    CMV phsA;
    CMV phsB;
    CMV phsC;
    
    WYE(string DataObjectName_, string LNRef_, bool Presence_);
    array<CMV,3> sendData();
};

#endif
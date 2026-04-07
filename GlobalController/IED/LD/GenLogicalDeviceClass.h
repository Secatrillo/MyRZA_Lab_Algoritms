#ifndef GENLOGICALDEVICECLASS_H
#define GENLOGICALDEVICECLASS_H

#include "../../../include.h"

class GenLogicalDeviceClass
{ //General class for logical devices
public:
    string LDName; //LDName - имя логического устройства
    GenLogicalDeviceClass(string LDName_);
};

#endif
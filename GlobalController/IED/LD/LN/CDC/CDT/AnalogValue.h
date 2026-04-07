#ifndef ANALOG_VALUE_H
#define ANALOG_VALUE_H

#include "../GenDataAttributeClass.h"
#include "TriggerOption.h"
#include <string>

using namespace std;

class AnalogValue: public GenDataAttributeClass
{
public:
    GenDataAttributeClass f;
    AnalogValue(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_,TriggerOption TrgOp_, string DataObjectRef_);

    void setNewValue(double newValue);
    double getValue();
};

#endif
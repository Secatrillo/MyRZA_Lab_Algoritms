#ifndef VECTOR_H
#define VECTOR_H

#include "AnalogValue.h"
#include "../GenDataAttributeClass.h"
#include "EnumFunctionalConstraints.h"
#include "TriggerOption.h"
#include "../../../../../../include.h"

class Vector : public GenDataAttributeClass
{

public:
    AnalogValue mag;

    Vector(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, string DataObjectRef_);

    void setMag(double newValue);
    double getMag();

};

#endif
#pragma once

#include "AnalogValue.h"
#include "GenDataAttributeClass.h"
#include "EnumFunctionalConstraints.h"
#include "TriggerOption.h"
#include <string>
#include <memory>

class Vector : public GenDataAttributeClass{
    public:


        Vector(std::string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, std::string DataObjectRef_);

        void setMag(double newValue);
        double getMag();
    private:
        std::unique_ptr<AnalogValue> mag;

};

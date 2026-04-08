#pragma once

#include "GenDataAttributeClass.h"
#include "TriggerOption.h"
#include <string>
#include <memory>



class AnalogValue: public GenDataAttributeClass {
    public:
        std::unique_ptr<GenDataAttributeClass> f;
        AnalogValue(std::string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_,TriggerOption TrgOp_, std::string DataObjectRef_);

        void setNewValue(double newValue);
        double getValue();
};


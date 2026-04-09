#include "AnalogValue.h"
#include "TriggerOption.h"
#include <iostream>
#include <string>
//Attribute type for analogue value

//f - значение измеряемой величины типа FLOAT32
AnalogValue::AnalogValue(std::string DataAttributeName_, 
                         EnumFunctionalConstraints FunctionalConstraint_,
                         TriggerOption TrgOp_, 
                         std::string DataObjectRef_):

    GenDataAttributeClass(DataAttributeName_,
                          FunctionalConstraint_, 
                          TrgOp_, 
                          "AnalogValue", 
                          "", 
                          0.0, 
                          DataObjectRef_),
    f (std::make_unique<GenDataAttributeClass>("f", 
        EFC::MX, 
        TrgOp_,
        "FLOAT32", 
        "", 
        0.0, 
        this->getDataAttributeRef()))
    {}

// Метод для задания значения атрибута типа AnalogValue
void AnalogValue::setNewValue(double newValue){
    f->setvalue(newValue);
}

//Метод для получения значения атрибута
double AnalogValue::getvalue(){
    return f->getvalue();
}


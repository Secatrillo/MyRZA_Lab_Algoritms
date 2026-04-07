#include "AnalogValue.h"
#include "TriggerOption.h"
#include <iostream>
#include <string>
//Attribute type for analogue value
using namespace std;
//f - значение измеряемой величины типа FLOAT32
AnalogValue::AnalogValue(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_,
                         TriggerOption TrgOp_, string DataObjectRef_):
    GenDataAttributeClass(DataAttributeName_, FunctionalConstraint_, TrgOp_, "AnalogValue", "", 0.0, DataObjectRef_),
    f("f", EnumFunctionalConstraints::MX, TrgOp_,"FLOAT32", "", 0.0, this->DataAttributeRef)
    {}

void AnalogValue::setNewValue(double newValue) //Метод для задания значения атрибута типа AnalogValue
{
    f.value = newValue;
}

double AnalogValue::getValue() //Метод для получения значения атрибута
{
    return f.value;
}


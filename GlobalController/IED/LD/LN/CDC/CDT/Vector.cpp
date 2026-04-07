#include "Vector.h"
//vector type class
//mag - амплитуда
Vector::Vector(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, string DataObjectRef_):
GenDataAttributeClass(DataAttributeName_, FunctionalConstraint_,  TrgOp_, "Vector", "", 0.0, DataObjectRef_), 
mag("mag", EnumFunctionalConstraints::SP,TriggerOption(true), DataObjectRef_)
{}

void Vector::setMag(double newValue) //Метод задания модуля комплексной величины
{
    mag.setNewValue(newValue);
}

double Vector::getMag() //Метод задания модуля комплексной величины
{
    return mag.getValue();
}

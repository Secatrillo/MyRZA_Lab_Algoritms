#include "Vector.h"
//vector type class
//mag - амплитуда
Vector::Vector(std::string DataAttributeName_,
               EFC FunctionalConstraint_, 
               TriggerOption TrgOp_, 
               std::string DataObjectRef_):
GenDataAttributeClass(DataAttributeName_, 
                      FunctionalConstraint_,  
                      TrgOp_, 
                      "Vector", 
                      "", 
                      0.0, 
                      DataObjectRef_), 
mag(std::make_unique<AnalogValue>("mag",
    EFC::SP,
    TriggerOption(true), 
    DataObjectRef_)),
ang(std::make_unique<AnalogValue>("ang",
    EFC::SP,
    TriggerOption(true), 
    DataObjectRef_))
{}

void Vector::setMag(double newValue){
    mag->setNewValue(newValue);
}

double Vector::getMag() const{
    return mag->getValue();
}

void Vector::setAng(double newValue){
    ang->setNewValue(newValue);
}

double Vector::getAng() const{
    return ang->getValue();
}

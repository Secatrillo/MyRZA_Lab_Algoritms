#include "Quality.h"
//Класс метки качества атрибута данных
Quality::Quality(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, string DataObjectRef_):
GenDataAttributeClass(DataAttributeName_, FunctionalConstraint_,TrgOp_, "Quality", "", 0.0, DataObjectRef_),
validity(ValidityEnum::good), overflow(false), outOfRange(false), badReference(false), oscillatory(false), failure(false), 
oldData(false), inconsistent(false), inaccurate(false), source(SourceEnum::process), test(false), operatorBlocked(false)
{
}
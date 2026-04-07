#include "TimeStamp.h"
//Класс типа метки времени атрибута данных
TimeStamp::TimeStamp(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, string DataObjectRef_):
GenDataAttributeClass(DataAttributeName_, FunctionalConstraint_, TrgOp_, "TimeStamp", "", 0.0, DataObjectRef_), SecondSinceEpoch(0), FractionOfSecond(0),
TimeQuality()
{
}
#include "ASG.h"
//analogue setting class
//setMag - Значение аналоговой уставки
ASG::ASG(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("ASG"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "ASG"),
setMag("setMag", EnumFunctionalConstraints::SP, TriggerOption(true), this->DataObjectRef)
{
}
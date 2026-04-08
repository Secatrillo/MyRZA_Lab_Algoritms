#include "ASG.h"
//analogue setting class
//setMag - Значение аналоговой уставки
ASG::ASG(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("ASG"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "ASG"),
setMag(std::make_unique<AnalogValue>("setMag", EnumFunctionalConstraints::SP, TriggerOption(true), this->getDataObjectRef()))
{
}
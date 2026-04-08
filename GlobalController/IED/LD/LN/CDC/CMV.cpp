#include "CMV.h"
//complex measured value class
//stVal - Основное значение
//q - метка качества
//t - метка времени
CMV::CMV(std::string SubDataObjectName_, bool Presence_, std::string DataObjectRef_):
GenCommonDataClass("CMV"),
GenDataObjectClass(SubDataObjectName_, DataObjectRef_,  Presence_, "CMV"),
cVal(std::make_unique<Vector>("cVal", EnumFunctionalConstraints::MX, TriggerOption(true,false,true), this->getDataObjectRef())),
q(std::make_unique<Quality>("q", EnumFunctionalConstraints::MX, TriggerOption(false, true, false), this->getDataObjectRef())),
t(std::make_unique<TimeStamp>("t", EnumFunctionalConstraints::MX, TriggerOption(), DataObjectRef_))
{
}
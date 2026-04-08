#include "INS.h"
//integer status class
//stVal - Основное значение
//q - метка качества
//t - метка времени
INS::INS(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("INS"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "INS"),
stVal(std::make_unique<GDAClass>("stVal", EnumFunctionalConstraints::ST, TriggerOption(true,false,true), "INT32")),
q(std::make_unique<Quality>("q", EnumFunctionalConstraints::ST, TriggerOption(false,true), this->getDataObjectRef())),
t(std::make_unique<TimeStamp>("t", EnumFunctionalConstraints::ST, TriggerOption(), this->getDataObjectRef()))
{
}
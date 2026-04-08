#include "ACT.h"
//protection activation information class
//general - Срабатывание защиты общее
//q - метка качества
//t - метка времени
//phsA - срабатывание защиты в фазе A
//phsB - срабатывание защиты в фазе B
//phsC - срабатывание защиты в фазе C
ACT::ACT(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("ACT"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "ACT"),
general(std::make_unique<GDAClass>("general", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0, this->getDataObjectRef())),
q(std::make_unique<Quality>("q", EnumFunctionalConstraints::ST, TriggerOption(false, true), this->getDataObjectRef())),
t(std::make_unique<TimeStamp>("t", EnumFunctionalConstraints::ST, TriggerOption(), this->getDataObjectRef())),
phsA(std::make_unique<GDAClass>("phsA", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0, this->getDataObjectRef())),
phsB(std::make_unique<GDAClass>("phsB", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0, this->getDataObjectRef())),
phsC(std::make_unique<GDAClass>("phsC", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0, this->getDataObjectRef()))
{
}
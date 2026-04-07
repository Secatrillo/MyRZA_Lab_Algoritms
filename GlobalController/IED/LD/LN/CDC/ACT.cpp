#include "ACT.h"
//protection activation information class
//general - Срабатывание защиты общее
//q - метка качества
//t - метка времени
//phsA - срабатывание защиты в фазе A
//phsB - срабатывание защиты в фазе B
//phsC - срабатывание защиты в фазе C
ACT::ACT(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("ACT"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "ACT"),
general("general", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0, this->DataObjectRef),
q("q", EnumFunctionalConstraints::ST, TriggerOption(false, true), this->DataObjectRef),
t("t", EnumFunctionalConstraints::ST, TriggerOption(), this->DataObjectRef),
phsA("phsA", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0, this->DataObjectRef),
phsB("phsB", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0, this->DataObjectRef),
phsC("phsC", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0, this->DataObjectRef)
{
}
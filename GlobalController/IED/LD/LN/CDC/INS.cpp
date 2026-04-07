#include "INS.h"
//integer status class
//stVal - Основное значение
//q - метка качества
//t - метка времени
INS::INS(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("INS"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "INS"),
stVal("stVal", EnumFunctionalConstraints::ST, TriggerOption(true,false,true), "INT32"),
q("q", EnumFunctionalConstraints::ST, TriggerOption(false,true), this->DataObjectRef),
t("t", EnumFunctionalConstraints::ST, TriggerOption(), this->DataObjectRef)
{
}
#include "DPC.h"
//controllable double point class
//stVal - Основное значение
//q - метка качества
//t - метка времени
//ctlModel - параметр, определяющий модель управлениями данными в соответствии с IEC61850-7-2
DPC::DPC(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("DPC"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "DPC"),
stVal("stVal", EnumFunctionalConstraints::ST, TriggerOption(true), "CODED ENUM", "intermediate-state|off|on|bad-state", 0.0, this->DataObjectRef),
q("q", EnumFunctionalConstraints::ST, TriggerOption(false,true), this->DataObjectRef),
t("t", EnumFunctionalConstraints::ST, TriggerOption(), this->DataObjectRef),
ctlModel("ctlModel", EnumFunctionalConstraints::CF, TriggerOption(true), "ENUMERATED",
"status-only|direct-with-normal-security|sbo-with-normal-security|directwith-enhanced-security|sbo-with-enhanced-security", 0.0, this->DataObjectRef)
{
}
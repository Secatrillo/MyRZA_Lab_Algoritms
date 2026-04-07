#include "SPC.h"
//controllable single point class
//stVal - Основное значение
//q - метка качества
//t - метка времени
//ctlModel - параметр, определяющий модель управлениями данными в соответствии с IEC61850-7-2
SPC::SPC(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("SPC"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "SPC"),
stVal("stVal", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0.0, this->DataObjectRef),
q("q", EnumFunctionalConstraints::ST, TriggerOption(false,true), this->DataObjectRef),
t("t", EnumFunctionalConstraints::ST, TriggerOption(), this->DataObjectRef),
ctlModel("ctlModel", EnumFunctionalConstraints::CF, TriggerOption(true), "ENUMERATED",
"status-only|direct-with-normal-security|sbo-with-normal-security|directwith-enhanced-security|sbo-with-enhanced-security")
{
}
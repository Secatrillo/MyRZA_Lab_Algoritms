#include "DPC.h"
//controllable double point class
//stVal - Основное значение
//q - метка качества
//t - метка времени
//ctlModel - параметр, определяющий модель управлениями данными в соответствии с IEC61850-7-2
DPC::DPC(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("DPC"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "DPC"),
stVal(std::make_unique<GDAClass>("stVal", EnumFunctionalConstraints::ST, TriggerOption(true), "CODED ENUM", "intermediate-state|off|on|bad-state", 0.0, this->getDataObjectRef())),
q(std::make_unique<Quality>("q", EnumFunctionalConstraints::ST, TriggerOption(false,true), this->getDataObjectRef())),
t(std::make_unique<TimeStamp>("t", EnumFunctionalConstraints::ST, TriggerOption(), this->getDataObjectRef())),
ctlModel(std::make_unique<GDAClass>("ctlModel", EnumFunctionalConstraints::CF, TriggerOption(true), "ENUMERATED",
"status-only|direct-with-normal-security|sbo-with-normal-security|directwith-enhanced-security|sbo-with-enhanced-security", 0.0, this->getDataObjectRef()))
{
}
#include "SPC.h"
//controllable single point class
//stVal - Основное значение
//q - метка качества
//t - метка времени
//ctlModel - параметр, определяющий модель управлениями данными в соответствии с IEC61850-7-2
SPC::SPC(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("SPC"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "SPC"),
stVal(std::make_unique<GDAClass>("stVal", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0.0, this->getDataObjectRef())),
q(std::make_unique<Quality>("q", EnumFunctionalConstraints::ST, TriggerOption(false,true), this->getDataObjectRef())),
t(std::make_unique<TimeStamp>("t", EnumFunctionalConstraints::ST, TriggerOption(), this->getDataObjectRef())),
ctlModel(std::make_unique<GDAClass>("ctlModel", EnumFunctionalConstraints::CF, TriggerOption(true), "ENUMERATED",
"status-only|direct-with-normal-security|sbo-with-normal-security|directwith-enhanced-security|sbo-with-enhanced-security"))
{
}
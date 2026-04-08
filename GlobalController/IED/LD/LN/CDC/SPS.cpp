#include "SPS.h"
//single point status class
//stVal - Основное значение
//q - метка качества
//t - метка времени
SPS::SPS(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("SPS"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "SPS"),
stVal(std::make_unique<GDAClass>("stVal", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN")),
q(std::make_unique<Quality>("q", EnumFunctionalConstraints::ST, TriggerOption(false, true), this->getDataObjectRef())),
t(std::make_unique<TimeStamp>("t", EnumFunctionalConstraints::ST, TriggerOption(), this->getDataObjectRef()))
{
}
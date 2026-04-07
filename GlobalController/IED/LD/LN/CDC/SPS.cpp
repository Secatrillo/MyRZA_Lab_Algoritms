#include "SPS.h"
//single point status class
//stVal - Основное значение
//q - метка качества
//t - метка времени
SPS::SPS(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("SPS"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "SPS"),
stVal("stVal", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN"),
q("q", EnumFunctionalConstraints::ST, TriggerOption(false, true), this->DataObjectRef),
t("t", EnumFunctionalConstraints::ST, TriggerOption(), this->DataObjectRef)
{
}
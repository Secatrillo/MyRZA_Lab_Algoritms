#include "CMV.h"
//complex measured value class
//stVal - Основное значение
//q - метка качества
//t - метка времени
CMV::CMV(string SubDataObjectName_, bool Presence_, string DataObjectRef_):
GenCommonDataClass("CMV"),
GenDataObjectClass(SubDataObjectName_, DataObjectRef_,  Presence_, "CMV"),
cVal("cVal", EnumFunctionalConstraints::MX, TriggerOption(true,false,true), this->DataObjectRef),
q("q", EnumFunctionalConstraints::MX, TriggerOption(false, true, false), this->DataObjectRef),
t("t", EnumFunctionalConstraints::MX, TriggerOption(), DataObjectRef_)
{
}
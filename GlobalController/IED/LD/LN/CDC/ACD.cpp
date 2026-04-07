#include "ACD.h"
//directed protection activation information class
//general - Срабатывание направленной защиты общее
//dirGeneral - Направленность срабатывания общее
//q - метка качества
//t - метка времени
//phsA - срабатывание защиты в фазе A
//phsB - срабатывание защиты в фазе B
//phsC - срабатывание защиты в фазе C
ACD::ACD(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("ACD"),
GenDataObjectClass(DataObjectName_, LNRef_+"/"+DataObjectName_, Presence_, "ACD"),
general("general", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0),
dirGeneral("dirGeneral", EnumFunctionalConstraints::ST, TriggerOption(false, true), "ENUMERATED", "unknown| forward | backward | both"),
q("q", EnumFunctionalConstraints::ST, TriggerOption(false, true), this->DataObjectRef),
t("t", EnumFunctionalConstraints::ST, TriggerOption(), this->DataObjectRef),
phsA("phsA", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN","", 0),
phsB("phsB", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN","", 0),
phsC("phsC", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN","", 0)
{
}
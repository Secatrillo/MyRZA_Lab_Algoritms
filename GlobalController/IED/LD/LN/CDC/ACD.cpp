#include "ACD.h"
//directed protection activation information class
//general - Срабатывание направленной защиты общее
//dirGeneral - Направленность срабатывания общее
//q - метка качества
//t - метка времени
//phsA - срабатывание защиты в фазе A
//phsB - срабатывание защиты в фазе B
//phsC - срабатывание защиты в фазе C
ACD::ACD(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("ACD"),
GenDataObjectClass(DataObjectName_, LNRef_+"/"+DataObjectName_, Presence_, "ACD"),
general(std::make_unique<GDAClass>("general", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN", "", 0)),
dirGeneral(std::make_unique<GDAClass>("dirGeneral", EnumFunctionalConstraints::ST, TriggerOption(false, true), "ENUMERATED", "unknown| forward | backward | both")),
q(std::make_unique<Quality>("q", EnumFunctionalConstraints::ST, TriggerOption(false, true), this->getDataObjectRef())),
t(std::make_unique<TimeStamp>("t", EnumFunctionalConstraints::ST, TriggerOption(), this->getDataObjectRef())),
phsA(std::make_unique<GDAClass>("phsA", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN","", 0)),
phsB(std::make_unique<GDAClass>("phsB", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN","", 0)),
phsC(std::make_unique<GDAClass>("phsC", EnumFunctionalConstraints::ST, TriggerOption(true), "BOOLEAN","", 0))
{
}
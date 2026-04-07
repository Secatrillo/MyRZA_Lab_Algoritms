#include "SAV.h"
//sampled value class
//instMag - мгновенное значение
//q - метка качества

SAV::SAV(string DataObjectName_, string LogicNodeRef_, bool Presence_):
GenCommonDataClass("SAV"),
GenDataObjectClass(DataObjectName_,LogicNodeRef_ + "$" + DataObjectName_, Presence_,  "SAV"),
instMag("instMag", EnumFunctionalConstraints::MX, TriggerOption(), this->DataObjectRef),
q("q", EnumFunctionalConstraints::MX, TriggerOption(false,true,false), this->DataObjectRef)
{
}

void SAV::setInstMag(double newInstMag) //Метод задания значения мгновенной выборки
{
    instMag.setNewValue(newInstMag);
}

double SAV::getInstMag() //Метод получения значения мгновенной выборки
{
    return instMag.getValue();
}

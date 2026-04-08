#include "SAV.h"
//sampled value class
//instMag - мгновенное значение
//q - метка качества

SAV::SAV(std::string DataObjectName_, std::string LogicNodeRef_, bool Presence_):
    GenCommonDataClass("SAV"),
    GenDataObjectClass(DataObjectName_,LogicNodeRef_ + "$" + DataObjectName_, Presence_,  "SAV"),
    instMag(std::make_unique<AnalogValue>("instMag", EnumFunctionalConstraints::MX, TriggerOption(), this->getDataObjectRef())),
    q(std::make_unique<Quality>("q", EnumFunctionalConstraints::MX, TriggerOption(false,true,false), this->getDataObjectRef()))
{
}

void SAV::setInstMag(double newInstMag) //Метод задания значения мгновенной выборки
{
    instMag->setNewValue(newInstMag);
}

double SAV::getInstMag() //Метод получения значения мгновенной выборки
{
    return instMag->getvalue();
}

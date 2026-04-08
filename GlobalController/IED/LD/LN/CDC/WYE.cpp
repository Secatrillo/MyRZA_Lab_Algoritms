#include "WYE.h"
//Phase to ground/neutral related measured values of a three-phase system class
//phsA - измеряемая величина фазы А;
//phsB - измеряемая величина фазы B;
//phsC - измеряемая величина фазы C;

WYE::WYE(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("WYE"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "WYE"),
phsA(std::make_shared<CMV>("phsA", false, this->getDataObjectRef())),
phsB(std::make_shared<CMV>("phsB", false, this->getDataObjectRef())),
phsC(std::make_shared<CMV>("phsC", false, this->getDataObjectRef()))
{
}

std::array<std::shared_ptr<CMV>, 3> WYE::sendData() //Метод для передачи данных трёхфазного измерения
{
    return std::array<std::shared_ptr<CMV>, 3>{phsA,phsB,phsC};
}

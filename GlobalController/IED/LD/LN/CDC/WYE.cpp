#include "WYE.h"
//Phase to ground/neutral related measured values of a three-phase system class
//phsA - измеряемая величина фазы А;
//phsB - измеряемая величина фазы B;
//phsC - измеряемая величина фазы C;

WYE::WYE(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("WYE"),
GenDataObjectClass(DataObjectName_, LNRef_+"$"+DataObjectName_, Presence_, "WYE"),
phsA("phsA", false, this->DataObjectRef),
phsB("phsB", false, this->DataObjectRef),
phsC("phsC", false, this->DataObjectRef)
{
}

array<CMV, 3> WYE::sendData() //Метод для передачи данных трёхфазного измерения
{
    return array<CMV, 3>{phsA,phsB,phsC};
}

#include "CTRL.h"
//Логическое устройство управления выключателем
//XCBR1 - Логический узел выключателя
//CSWI1 - логической узел управления силовым выключателем
CTRL::CTRL(std::string LDName_):
GenLogicalDeviceClass(LDName_),
CSWI1("CSWI1", LDName_),
XCBR1("XCBR1", LDName_)
{
}

void CTRL::receiveTripSignal(std::shared_ptr<ACT> TripSignal)
{ //Получение сигнала TRIP из логического устройства защиты
    CSWI1.receiveTrip(TripSignal);
    CSWI1.Pos = XCBR1.Pos;
}

void CTRL::actOnSignal()
{ //Получение сигнала TRIP из логического устройства защиты
    XCBR1.actOnOpen(CSWI1.OpOpn);
}

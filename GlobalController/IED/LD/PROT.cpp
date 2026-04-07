#include "PROT.h"
//Логическое устройство защиты
//PTOC1 - Логический узел ТО без ВВ
//PTOC2 - Логический узел ТО с ВВ (МТЗ)
//PTRC1 - Логический узел формирования сигнала на отключение
PROT::PROT(string LDName_):
GenLogicalDeviceClass(LDName_),
PTOC1("PTOC1", LDName_),
PTOC2("PTOC2", LDName_),
PTRC1("PTRC1", LDName_)
{
}

void PROT::setSettings(double tmOpPTOC1, double currentOpPTOC1, double tmOpPTOC2, double currentOpPTOC2)
{ //Метод задания уставок токовых защит
    PTOC1.setStrVal(currentOpPTOC1);
    PTOC1.setOpDlTmms(tmOpPTOC1);
    PTOC2.setStrVal(currentOpPTOC2);
    PTOC2.setOpDlTmms(tmOpPTOC2);
}

void PROT::acceptDataFromMMXU(WYE wye)
{ //Приём данных от устройства измерений
    PTOC1.acceptDataFromMMXU(wye);
    PTOC2.acceptDataFromMMXU(wye);
}


void PROT::imitateRP(double timedat)
{ //Имитация работы РЗ
    PTOC1.checkStr(timedat);
    PTOC2.checkStr(timedat);
    PTOC1.checkReturn();
    PTOC2.checkReturn();
    PTOC1.checkTimeStr(timedat);
    PTOC2.checkTimeStr(timedat);
    if (PTOC1.Op.general.value == 1)
    {
        PTOC2.Op.general.value = 0;
        PTOC2.Str.general.value = 0;
        PTOC2.Str.phsA.value = 0;
        PTOC2.Str.phsB.value = 0;
        PTOC2.Str.phsC.value = 0;
    }
    
    
    // создаем локальный вектор перед вызовом
    vector<ACT*> ops = { &PTOC1.Op, &PTOC2.Op };
    PTRC1.formTrip(ops);
}




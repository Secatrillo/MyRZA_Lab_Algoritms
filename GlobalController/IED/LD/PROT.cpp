#include "PROT.h"
//Логическое устройство защиты
//PTOC1 - Логический узел ТО без ВВ
//PTOC2 - Логический узел ТО с ВВ (МТЗ)
//PTRC1 - Логический узел формирования сигнала на отключение
PROT::PROT(std::string LDName_):
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

void PROT::acceptDataFromMSQI(std::shared_ptr<CMV> data)
{ //Приём данных от устройства измерений
    PTOC1.acceptDataFromMSQI(data);
    PTOC2.acceptDataFromMSQI(data);
}


void PROT::imitateRP(double timedat)
{ //Имитация работы РЗ
    PTOC1.checkStr(timedat);
    PTOC2.checkStr(timedat);
    PTOC1.checkReturn();
    PTOC2.checkReturn();
    PTOC1.checkTimeStr(timedat);
    PTOC2.checkTimeStr(timedat);
    // if (PTOC1.Op->general->getvalue() == 1)
    // {
    //     PTOC2.Op->general->setvalue(0);
    //     PTOC2.Str->general->setvalue(0);
    //     PTOC2.Str->phsA->setvalue(0);
    //     PTOC2.Str->phsB->setvalue(0);
    //     PTOC2.Str->phsC->setvalue(0);
    // }
    
    
    // создаем локальный вектор перед вызовом
    
    PTRC1.formTrip({ PTOC1.Op, PTOC2.Op });
    
}




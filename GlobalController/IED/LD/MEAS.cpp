#include "MEAS.h"
//Логическое устройство измерений
//LSVS1 - Логический узел приёма SV сообщений
//MMXU1 - Логический узел измерения полученных мгновенных выборок
//Fourier1 - Логический узел фильтра Фурье
MEAS::MEAS(std::string LDName_):
GenLogicalDeviceClass(LDName_),
LSVS1("LSVS1", "MEAS"),
MMXU1("MMXU1", "MEAS"),
Fourier1("Fourier1", "MEAS"),
MSQI1("MSQI1","MSQI")
{
}

void MEAS::setFourierMode(bool isFourier, int discrit){
    Fourier1.mode->stVal->setvalue(isFourier);
}

void MEAS::acceptSV(std::shared_ptr<std::vector<double>> sv)
{ //Метод приёма SV сообщения извне
    LSVS1.acceptIncomingSV(sv);
}

void MEAS::sendLSVSDataToMMXU()
{ //Передача мгновенных значений от LSVS1 в MMXU1
    MMXU1.currentA->setInstMag(LSVS1.currentA->getInstMag()); 
    MMXU1.currentB->setInstMag(LSVS1.currentB->getInstMag());
    MMXU1.currentC->setInstMag(LSVS1.currentC->getInstMag());
}

void MEAS::calculateFourier()
{ //Расчёт преобразования Фурье
    Fourier1.recieveSampledValues(MMXU1.currentA->getInstMag(), MMXU1.currentB->getInstMag(), MMXU1.currentC->getInstMag());
    
}

void MEAS::sendFourierDataToMMXU()
{ //Передача выхода фильтра Фурье в логический узел измерений
    MMXU1.A->phsA->set_cVal(Fourier1.fourierA->get_cVal());
    MMXU1.A->phsB->set_cVal(Fourier1.fourierB->get_cVal());
    MMXU1.A->phsC->set_cVal(Fourier1.fourierC->get_cVal());
}

void MEAS::sendMMXUDataToMSQI()
{ //Передача выхода фильтра Фурье в логический узел рассчёта последовательностей
    MSQI1.A->phsA->set_cVal(MMXU1.A->phsA->get_cVal());
    MSQI1.A->phsB->set_cVal(MMXU1.A->phsB->get_cVal());
    MSQI1.A->phsC->set_cVal(MMXU1.A->phsC->get_cVal());
}

void MEAS::calculateSequenses()
{ //Расчёт Последовательностей
    MSQI1.Calculate();
}



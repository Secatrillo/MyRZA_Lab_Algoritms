#include "MEAS.h"
//Логическое устройство измерений
//LSVS1 - Логический узел приёма SV сообщений
//MMXU1 - Логический узел измерения полученных мгновенных выборок
//Fourier1 - Логический узел фильтра Фурье
MEAS::MEAS(std::string LDName_):
GenLogicalDeviceClass(LDName_),
LSVS1("LSVS1", "MEAS"),
MMXU1("MMXU1", "MEAS"),
Fourier1("Fourier1", "MEAS")
{
}

void MEAS::acceptSV(std::vector<double> sv)
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
    Fourier1.calculateFourier();
}

void MEAS::sendFourierDataToMMXU()
{ //Передача выхода фильтра Фурье в логический узел измерений
    MMXU1.A.phsA->cVal->setMag(Fourier1.fourierA->getMag());
    MMXU1.A.phsB->cVal->setMag(Fourier1.fourierB->getMag());
    MMXU1.A.phsC->cVal->setMag(Fourier1.fourierC->getMag());
}



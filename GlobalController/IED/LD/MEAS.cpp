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

void MEAS::setSettings(bool isFourier, int discrit, std::shared_ptr<ParserComtrade> parser){
    Fourier1.mode->stVal->setvalue(isFourier);
    LSVS1.setParserData(parser);

}

void MEAS::acceptSV(int& i)
{ //Метод приёма SV сообщения извне
    LSVS1.acceptIncomingSV(i);
}

void MEAS::sendLSVSDataToMMXU()
{ //Передача мгновенных значений от LSVS1 в MMXU1
    // MMXU1.currentA->setInstMag(LSVS1.currentA->getInstMag()); 
    // MMXU1.currentB->setInstMag(LSVS1.currentB->getInstMag());
    // MMXU1.currentC->setInstMag(LSVS1.currentC->getInstMag());
    MMXU1.currentA = LSVS1.currentA; 
    MMXU1.currentB = LSVS1.currentB;
    MMXU1.currentC = LSVS1.currentC;
}

void MEAS::calculateFourier()
{ //Расчёт преобразования Фурье
    Fourier1.recieveSampledValues(MMXU1.currentA->getInstMag(), MMXU1.currentB->getInstMag(), MMXU1.currentC->getInstMag());
    
}

void MEAS::sendFourierDataToMMXU()
{ //Передача выхода фильтра Фурье в логический узел измерений
    MMXU1.A->phsA = Fourier1.fourierA;
    MMXU1.A->phsB = Fourier1.fourierB;
    MMXU1.A->phsC = Fourier1.fourierC;
}

void MEAS::sendMMXUDataToMSQI()
{ //Передача выхода фильтра Фурье в логический узел рассчёта последовательностей
    MSQI1.A->phsA = MMXU1.A->phsA;
    MSQI1.A->phsB = MMXU1.A->phsB;
    MSQI1.A->phsC = MMXU1.A->phsC;
}

void MEAS::calculateSequenses()
{ //Расчёт Последовательностей
    MSQI1.Calculate();
}



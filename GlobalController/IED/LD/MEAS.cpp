#include "MEAS.h"
//Логическое устройство измерений
//LSVS1 - Логический узел приёма SV сообщений
//MMXU1 - Логический узел измерения полученных мгновенных выборок
//Fourier1 - Логический узел фильтра Фурье
MEAS::MEAS(string LDName_):
GenLogicalDeviceClass(LDName_),
LSVS1("LSVS1", "MEAS"),
MMXU1("MMXU1", "MEAS"),
Fourier1("Fourier1", "MEAS")
{
}

void MEAS::acceptSV(vector<double> sv)
{ //Метод приёма SV сообщения извне
    LSVS1.acceptIncomingSV(sv);
}

void MEAS::sendLSVSDataToMMXU()
{ //Передача мгновенных значений от LSVS1 в MMXU1
    MMXU1.currentA.instMag.f.value = LSVS1.currentA.instMag.f.value;
    MMXU1.currentB.instMag.f.value = LSVS1.currentB.instMag.f.value;
    MMXU1.currentC.instMag.f.value = LSVS1.currentC.instMag.f.value;
}

void MEAS::calculateFourier()
{ //Расчёт преобразования Фурье
    Fourier1.recieveSampledValues(MMXU1.currentA.instMag.f.value, MMXU1.currentB.instMag.f.value, MMXU1.currentC.instMag.f.value);
    Fourier1.calculateFourier();
}

void MEAS::sendFourierDataToMMXU()
{ //Передача выхода фильтра Фурье в логический узел измерений
    MMXU1.A.phsA.cVal.mag.setNewValue(Fourier1.fourierA.mag.f.value);
    MMXU1.A.phsB.cVal.mag.setNewValue(Fourier1.fourierB.mag.f.value);
    MMXU1.A.phsC.cVal.mag.setNewValue(Fourier1.fourierC.mag.f.value);
}



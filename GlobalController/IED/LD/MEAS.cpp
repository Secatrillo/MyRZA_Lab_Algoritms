#include "MEAS.h"

MEAS::MEAS(std::string LDName_)
    : GenLogicalDeviceClass(LDName_),
      LSVS_I("LSVS_I", "MEAS"),
      LSVS_U("LSVS_U", "MEAS"),
      mmxuV2("MMXU_V2", "MEAS"),
      Fourier_I("Fourier_I", "MEAS"),
      Fourier_U("Fourier_U", "MEAS"),
      MSQI1("MSQI1", "MSQI")
{
    mmxuV2.bindInstantFromLSVS(LSVS_I, LSVS_U);
}

void MEAS::setSettings(bool isFourier, int /*discrit*/, std::shared_ptr<ParserComtrade> parserCurrent,
                       std::shared_ptr<ParserComtrade> parserVoltage)
{
    Fourier_I.mode->stVal->setvalue(isFourier);
    Fourier_U.mode->stVal->setvalue(isFourier);
    LSVS_I.setParserData(parserCurrent);
    LSVS_U.setParserData(parserVoltage);
}

void MEAS::acceptSV(int& i)
{
    LSVS_I.acceptIncomingSV(i);
    LSVS_U.acceptIncomingSV(i);
}

void MEAS::calculateFourier_V2()
{
    if (!mmxuV2.currentA || !mmxuV2.voltageAB)
        return;
    Fourier_I.recieveSampledValues(mmxuV2.currentA->getInstMag(), mmxuV2.currentB->getInstMag(),
                                   mmxuV2.currentC->getInstMag());
    Fourier_U.recieveSampledValues(mmxuV2.voltageAB->getInstMag(), mmxuV2.voltageBC->getInstMag(),
                                   mmxuV2.voltageCA->getInstMag());
}

void MEAS::sendFourierDataToMMXU_V2()
{
    mmxuV2.applyFourierOutputs(Fourier_I.fourierA, Fourier_I.fourierB, Fourier_I.fourierC, Fourier_U.fourierA,
                               Fourier_U.fourierB, Fourier_U.fourierC);
    mmxuV2.recalculateImpedance();
}

void MEAS::sendMMXU_V2_DataToMSQI()
{
    MSQI1.A->phsA = Fourier_I.fourierA;
    MSQI1.A->phsB = Fourier_I.fourierB;
    MSQI1.A->phsC = Fourier_I.fourierC;
}

void MEAS::calculateSequenses()
{
    MSQI1.Calculate();
}

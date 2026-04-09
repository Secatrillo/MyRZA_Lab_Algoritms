#include "IED.h"

IED::IED(std::string IEDName_):
meas("MEAS"),
prot("PROT"),
ctrl("CTRL"),
dr("DR")
{
    IEDName=IEDName_;
}

void IED::setSettings(double StrValPTOC1, double OpDlTmmsPTOC1, double StrValPTOC2, double OpDlTmmsPTOC2, bool FourierMode, int discrit)
{
    prot.setSettings(OpDlTmmsPTOC1, StrValPTOC1, OpDlTmmsPTOC2, StrValPTOC2);
    meas.setFourierMode(FourierMode, discrit);
}

void IED::modelIEDWork(std::shared_ptr<std::vector<double>> SVMessage, double timeValues)
{
    // GlobalTimeController::t = timeValues;
    // 2. Приём данных и расчёты в meas
    meas.acceptSV(SVMessage);
    meas.sendLSVSDataToMMXU();
    meas.calculateFourier();
    meas.sendFourierDataToMMXU();
    meas.sendMMXUDataToMSQI();
    meas.calculateSequenses();
    // 3. Передача данных в защиту
    prot.acceptDataFromMSQI(meas.MSQI1.ZeroSeq);
    prot.imitateRP(timeValues);
    // 4. Передача сигнала Trip в устройство управления
    ctrl.receiveTripSignal(prot.PTRC1.Tr);
    // 5. Регистрируем данные в dr
    std::vector<double> MMXUmeas = {
        meas.MMXU1.A->phsA->cVal->getMag(),
        meas.MMXU1.A->phsB->cVal->getMag(),
        meas.MMXU1.A->phsC->cVal->getMag()
    };
    
    MMXUData = MMXUmeas;

    dr.registerData(SVMessage, MMXUmeas, timeValues, 
    prot.PTOC1.Str->general->getvalue(),
    prot.PTOC1.Str->phsA->getvalue(),
    prot.PTOC1.Str->phsB->getvalue(),
    prot.PTOC1.Str->phsC->getvalue(),
    prot.PTOC1.Op->general->getvalue(),
    prot.PTOC2.Str->general->getvalue(),
    prot.PTOC2.Str->phsA->getvalue(),
    prot.PTOC2.Str->phsB->getvalue(),
    prot.PTOC2.Str->phsC->getvalue(),
    prot.PTOC2.Op->general->getvalue(),
    prot.PTRC1.Tr->general->getvalue());
}

std::vector<double> IED::IEDAntireciverData()
{
    // 3. Передача данных IED
    std::vector<double> MMXUOtherData;
    MMXUOtherData = {
        meas.MMXU1.A->phsA->cVal->getMag(),
        meas.MMXU1.A->phsB->cVal->getMag(),
        meas.MMXU1.A->phsC->cVal->getMag(),
    };   
    MMXUData = MMXUOtherData;    
    return MMXUData;
}

void IED::IEDReciverData(double timeValues, std::vector<double> MMXUOtherMeas)
{
    dr.registerOtherData(MMXUOtherMeas, timeValues);
}

void IED::watchOscill(ParserComtrade parser_, std::string filename_, std::string plotscriptFile_, std::string pngFile_)
{
    dr.exportForGnuplot(parser_, filename_);
    dr.showPlot(filename_, plotscriptFile_, pngFile_);
}



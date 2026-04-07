#include "IED.h"

IED::IED(string IEDName_):
MEAS("MEAS"),
PROT("PROT"),
CTRL("CTRL"),
DR("DR")
{
    IEDName=IEDName_;
}

void IED::setProtSettings(double StrValPTOC1, double OpDlTmmsPTOC1, double StrValPTOC2, double OpDlTmmsPTOC2)
{
    PROT.setSettings(OpDlTmmsPTOC1,StrValPTOC1, OpDlTmmsPTOC2, StrValPTOC2);
}

void IED::modelIEDWork(vector<double> SVMessage, double timeValues)
{
    // GlobalTimeController::t = timeValues;
    // 2. Приём данных и расчёты в MEAS
    MEAS.acceptSV(SVMessage);
    MEAS.sendLSVSDataToMMXU();
    MEAS.calculateFourier();
    MEAS.sendFourierDataToMMXU();
    // 3. Передача данных в защиту
    PROT.acceptDataFromMMXU(MEAS.MMXU1.A);
    PROT.imitateRP(timeValues);
    // 4. Передача сигнала Trip в устройство управления
    CTRL.receiveTripSignal(PROT.PTRC1.Tr);
    // 5. Регистрируем данные в DR
    vector<double> MMXUMeas = {
    MEAS.MMXU1.A.phsA.cVal.mag.f.value,
    MEAS.MMXU1.A.phsB.cVal.mag.f.value,
    MEAS.MMXU1.A.phsC.cVal.mag.f.value};
    
    MMXUData = MMXUMeas;

    DR.registerData(SVMessage, MMXUMeas, timeValues, 
    PROT.PTOC1.Str.general.value,
    PROT.PTOC1.Str.phsA.value,
    PROT.PTOC1.Str.phsB.value,
    PROT.PTOC1.Str.phsC.value,
    PROT.PTOC1.Op.general.value,
    PROT.PTOC2.Str.general.value,
    PROT.PTOC2.Str.phsA.value,
    PROT.PTOC2.Str.phsB.value,
    PROT.PTOC2.Str.phsC.value,
    PROT.PTOC2.Op.general.value,
    PROT.PTRC1.Tr.general.value);
}

vector<double> IED::IEDAntireciverData()
{
    // 3. Передача данных IED
    vector<double> MMXUOtherData;
    MMXUOtherData = {
        MEAS.MMXU1.A.phsA.cVal.mag.f.value,
        MEAS.MMXU1.A.phsB.cVal.mag.f.value,
        MEAS.MMXU1.A.phsC.cVal.mag.f.value,
    };   
    MMXUData = MMXUOtherData;    
    return MMXUData;
}

void IED::IEDReciverData(double timeValues, vector<double> MMXUOtherMeas)
{
    DR.registerOtherData(MMXUOtherMeas, timeValues);
}

void IED::watchOscill(ParserComtrade parser_, string filename_, string plotscriptFile_, string pngFile_)
{
    DR.exportForGnuplot(parser_, filename_);
    DR.showPlot(filename_, plotscriptFile_, pngFile_);
}



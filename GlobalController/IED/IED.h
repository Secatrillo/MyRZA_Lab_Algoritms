#pragma once

#include "LD/PROT.h"
#include "LD/CTRL.h"
#include "LD/DR.h"
#include "LD/MEAS.h"
#include "../../include.h"
#include "../../ExLabLibs/ParserComtrade.h"

class IED {
    public:
        IED(std::string IEDName_);
        void setSettings(double StrValPTOC1, double OpDlTmmsPTOC1, double StrValPTOC2, double OpDlTmmsPTOC2, bool FourierMode, int discrit);
        void modelIEDWork(std::shared_ptr<std::vector<double>> SVMessage, double timeValues);
        void watchOscill(ParserComtrade parser_, std::string filename_, std::string plotscriptFile_, std::string pngFile_);
        std::vector<double> IEDAntireciverData();
        void IEDReciverData(double timeValues, std::vector<double> MMXUOtherMeas);

    private:
        std::string IEDName;
        MEAS meas;
        PROT prot;
        CTRL ctrl;
        DR dr;
        std::vector<double> MMXUData;
};


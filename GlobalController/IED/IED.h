#ifndef IED_H
#define IED_H

#include "LD/PROT.h"
#include "LD/CTRL.h"
#include "LD/DR.h"
#include "LD/MEAS.h"
// #include "GlobalTimeController.h"
#include "../../include.h"
#include "../../ExLabLibs/ParserComtrade.h"

class IED
{
public:
    string IEDName;
    MEAS MEAS;
    PROT PROT;
    CTRL CTRL;
    DR DR;
    vector<double> MMXUData;
    IED(string IEDName_);
    void setProtSettings(double StrValPTOC1, double OpDlTmmsPTOC1, double StrValPTOC2, double OpDlTmmsPTOC2);
    void modelIEDWork(vector<double> SVMessage, double timeValues);
    void watchOscill(ParserComtrade parser_, string filename_, string plotscriptFile_, string pngFile_);
    vector<double> IEDAntireciverData();
    void IEDReciverData(double timeValues, vector<double> MMXUOtherMeas);
};

#endif
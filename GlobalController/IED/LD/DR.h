#ifndef DR_H
#define DR_H

#include "GenLogicalDeviceClass.h"
#include "LN/RADR.h"
#include "LN/RBDR.h"
#include "LN/RDRE.h"
#include "../../../include.h"
#include "../../../ExLabLibs/ParserComtrade.h"
#include <fstream>
#include <iomanip>

class DR : public GenLogicalDeviceClass
{
public:
    RDRE RDRE1;
    RADR RADR1;
    RADR RADR2;
    RADR RADR3;
    RADR RADR4;
    RADR RADR5;
    RADR RADR6;
    RADR RADR_otherA;
    RADR RADR_otherB;
    RADR RADR_otherC;
    
    RBDR RBDR1;
    RBDR RBDR2;
    RBDR RBDR3;
    RBDR RBDR4;
    RBDR RBDR5;
    RBDR RBDR6;
    RBDR RBDR7;
    RBDR RBDR8;
    RBDR RBDR9;
    RBDR RBDR10;
    RBDR RBDR11;

    vector<RADR*> RADRch;
    vector<RBDR*> RBDRch;

    DR(string LDName_);
    void registerData(vector<double> SampledValues,  vector<double> MMXUData,
    double svTime, bool PTOC1StrGen, bool PTOC1StrPhA, bool PTOC1StrPhB, bool PTOC1StrPhC, bool PTOC1OpGen, bool PTOC2StrGen, 
    bool PTOC2StrPhA, bool PTOC2StrPhB, bool PTOC2StrPhC, bool PTOC2OpGen, bool PTRCTrip);
    void registerOtherData(vector<double> MMXUData_, double svTime);
    void exportForGnuplot(const ParserComtrade& parser, const string& filename);
    void showPlot(string dataFile_, string plotscript_file_, string pngFile_);
};

#endif
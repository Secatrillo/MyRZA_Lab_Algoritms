#pragma once

#include "GenLogicalDeviceClass.h"
#include "LN/RADR.h"
#include "LN/RBDR.h"
#include "LN/RDRE.h"
#include <iostream>
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

    std::vector<RADR*> RADRch;
    std::vector<RBDR*> RBDRch;

    DR(std::string LDName_);
    void registerData(  std::vector<double> SampledValues,
                        std::vector<double> MMXUData,
                        double svTime,
                        bool PTOC1StrGen,
                        bool PTOC1StrPhA, 
                        bool PTOC1StrPhB, 
                        bool PTOC1StrPhC, 
                        bool PTOC1OpGen, 
                        bool PTOC2StrGen, 
                        bool PTOC2StrPhA, 
                        bool PTOC2StrPhB, 
                        bool PTOC2StrPhC, 
                        bool PTOC2OpGen, 
                        bool PTRCTrip);
    void registerOtherData(std::vector<double> MMXUData_, double svTime);
    void exportForGnuplot(const ParserComtrade& parser, const std::string& filename);
    void showPlot(std::string dataFile_, std::string plotscript_file_, std::string pngFile_);
};


#pragma once

#include "CDC/ACT.h"
#include "CDC/ACD.h"
#include "CDC/ASG.h"
#include "CDC/ING.h"
#include "CDC/CMV.h"
#include "GenLogicalNodeClass.h"

/**
 * Phasor Increment Comparator (приращение комплексной величины).
 *
 * Принимает CMV (последовательность от MSQI) как CurrentVec; на каждом такте
 * считает Δ = CurrentVec − PrevVec в комплексной форме. Если |Δ| превышает StrVal
 * или |∠Δ| превышает StrAng — пуск (Str) с фиксацией момента tStr; PrevVec при
 * этом НЕ обновляется (чтобы измерять длительность скачка относительно базы).
 * Если уставки не превышены — PrevVec ← CurrentVec; при уже выставленном Str
 * сброс выполняется с гистерезисом (см. PIOC.cpp), чтобы не срывать выдержку OpDlTmms.
 *
 * При истечении выдержки OpDlTmms и BlkDPP=false выставляется Op.
 */
class PIOC : public GenLogicalNodeClass
{
public:
    std::shared_ptr<CMV> CurrentVec; 
    std::unique_ptr<CMV> PrevVec;    

    std::shared_ptr<ASG> StrVal;     
    std::shared_ptr<ASG> StrAng;     
    std::shared_ptr<ING> OpDlTmms;   

    std::shared_ptr<ACT> Op;         
    std::shared_ptr<ACD> Str;        
    std::shared_ptr<ACT> BlkDPP;     

    double tStr = 0.0;
    bool prevValid = false;

    PIOC(std::string LogicalNodeName_, std::string LogicalDeviceRef_);

    void setStrVal(double strVal);
    void setStrAng(double strAngRad);
    void setOpDlTmms(double seconds);

    void acceptDataFromMSQI(std::shared_ptr<CMV> data);
    void acceptBlock(std::shared_ptr<ACT> blk);


    void checkStr(int sampleIndex, double timedat);
    void checkTimeStr(int sampleIndex, double timedat);

    static constexpr int kWarmupSamples = 80;
};

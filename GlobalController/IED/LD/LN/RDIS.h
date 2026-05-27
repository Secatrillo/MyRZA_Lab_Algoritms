#pragma once

#include "CDC/ACT.h"
#include "CDC/ACD.h"
#include "CDC/ASG.h"
#include "CDC/ING.h"
#include "CDC/CMV.h"
#include "GenLogicalNodeClass.h"

/**
 * Блокировка по приращению тока последовательности (аналог PIOC без внешней блокировки).
 * Выход Blk: true при отсутствии превышения уставки по приращению (блокировка дистанции активна);
 * false при превышении (блокировка снята, PDIS может оценивать зону).
 * Str — фиксация момента пуска (для диагностики); выдержка OpDlTmms на Blk не влияет.
 */
class RDIS : public GenLogicalNodeClass
{
public:
    std::shared_ptr<CMV> CurrentVec;
    std::unique_ptr<CMV> PrevVec;

    std::shared_ptr<ASG> StrVal;
    std::shared_ptr<ASG> StrAng;
    std::shared_ptr<ING> OpDlTmms;

    std::shared_ptr<ACT> Blk;
    std::shared_ptr<ACD> Str;

    double tStr = 0.0;
    bool prevValid = false;
    bool wasBlocked = false;
    double tPrev = 0.0;
    bool curValid = false;
    double lastCurMag = 0.0;
    double lastCurAng = 0.0;
    double tCur = 0.0;

    RDIS(std::string LogicalNodeName_, std::string LogicalDeviceRef_);

    void setStrVal(double strVal);
    void setStrAng(double strAngRad);
    void setOpDlTmms(double seconds);

    void acceptDataFromMSQI(const std::shared_ptr<CMV>& data);

    void checkStr(int sampleIndex, double timedat);
    void checkTimeStr(int sampleIndex, double timedat);

    static constexpr int kWarmupSamples = 300;
    static constexpr double kNominalFreqHz = 50.0;
    static constexpr double kEpsMag = 1e-9;
    static constexpr double kEpsAng = 1e-9;
};

#include "RDIS.h"

#include <cmath>
#include <complex>
#include <cstdint>

RDIS::RDIS(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_),
      CurrentVec(nullptr),
      PrevVec(std::make_unique<CMV>("Предыдущий вектор", false, this->getLNRef())),
      StrVal(std::make_shared<ASG>("Уставка по приращению модуля", this->getLNRef(), false)),
      StrAng(std::make_shared<ASG>("Уставка по приращению угла", this->getLNRef(), false)),
      OpDlTmms(std::make_shared<ING>("Уставка по времени блокировки", this->getLNRef(), false)),
      Blk(std::make_shared<ACT>("Блокировка дистанционной защиты", this->getLNRef(), false)),
      Str(std::make_shared<ACD>("Пуск блокировки", this->getLNRef(), false))
{}

void RDIS::setStrVal(double strVal)
{
    StrVal->setMag->f->setvalue(strVal);
}

void RDIS::setStrAng(double strAngRad)
{
    StrAng->setMag->f->setvalue(strAngRad);
}

void RDIS::setOpDlTmms(double seconds)
{
    OpDlTmms->setVal->setvalue(static_cast<int32_t>(seconds * 1e3));
}

void RDIS::acceptDataFromMSQI(const std::shared_ptr<CMV>& data)
{
    CurrentVec = data;
}

static double wrapAngleRdis(double a)
{
    while (a > M_PI)
        a -= 2.0 * M_PI;
    while (a < -M_PI)
        a += 2.0 * M_PI;
    return a;
}

static std::complex<double> rotateToSyncFrameRdis(double mag, double ang, double tSeconds)
{
    const double w = 2.0 * M_PI * RDIS::kNominalFreqHz;
    return std::polar(mag, ang - w * tSeconds);
}

void RDIS::checkStr(int sampleIndex, double timedat)
{
    if (!CurrentVec)
        return;

    const double curMag = CurrentVec->cVal->getMag();
    const double curAng = CurrentVec->cVal->getAng();

    if (!curValid) {
        curValid = true;
        lastCurMag = curMag;
        lastCurAng = curAng;
        tCur = timedat;
    } else {
        const bool magChanged = std::abs(curMag - lastCurMag) > kEpsMag;
        const bool angChanged = std::abs(wrapAngleRdis(curAng - lastCurAng)) > kEpsAng;
        if (magChanged || angChanged) {
            lastCurMag = curMag;
            lastCurAng = curAng;
            tCur = timedat;
        }
    }

    if (sampleIndex < kWarmupSamples) {
        PrevVec->cVal->setMag(curMag);
        PrevVec->cVal->setAng(curAng);
        prevValid = true;
        tPrev = tCur;
        Str->general->setvalue(false);
        Blk->general->setvalue(true);
        return;
    }

    if (!prevValid) {
        PrevVec->cVal->setMag(curMag);
        PrevVec->cVal->setAng(curAng);
        prevValid = true;
        tPrev = tCur;
        Str->general->setvalue(false);
        Blk->general->setvalue(true);
        return;
    }

    const double prevMag = PrevVec->cVal->getMag();
    const double prevAng = PrevVec->cVal->getAng();

    const std::complex<double> cur = rotateToSyncFrameRdis(curMag, curAng, tCur);
    const std::complex<double> prev = rotateToSyncFrameRdis(prevMag, prevAng, tPrev);
    const std::complex<double> diff = cur - prev;

    const double dMag = std::abs(diff);
    const double dPhi = std::abs(wrapAngleRdis(std::arg(cur) - std::arg(prev)));

    const double thrMag = StrVal->setMag->f->getvalue();
    const double thrAng = StrAng->setMag->f->getvalue();

    const bool angExceeded = (dMag > (thrMag * 0.1)) && (dPhi > thrAng);
    const bool exceeded = (dMag > thrMag) || angExceeded;

    if (exceeded) {
        if (!Str->general->getvalue()) {
            Str->general->setvalue(true);
            tStr = timedat;
        }
        /* Превышение по приращению — снять блокировку дистанции (Blk=false). */
        Blk->general->setvalue(false);
    } else {
        PrevVec->cVal->setMag(curMag);
        PrevVec->cVal->setAng(curAng);
        tPrev = tCur;
        Str->general->setvalue(false);
        /* Норма: блокировка дистанции активна (Blk=true). */
        Blk->general->setvalue(true);
    }
}

void RDIS::checkTimeStr(int sampleIndex, double timedat)
{
    (void)sampleIndex;
    (void)timedat;
    /* Blk выставляется в checkStr: норма → блокировка, превышение по приращению → снятие блокировки. */
}

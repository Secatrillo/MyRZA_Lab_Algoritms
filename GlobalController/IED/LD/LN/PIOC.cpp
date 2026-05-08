#include "PIOC.h"

#include <cmath>
#include <complex>
#include <cstdint>

PIOC::PIOC(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_),
      CurrentVec(nullptr),
      PrevVec(std::make_unique<CMV>("Предыдущий вектор", false, this->getLNRef())),
      StrVal(std::make_shared<ASG>("Уставка по приращению модуля", this->getLNRef(), false)),
      StrAng(std::make_shared<ASG>("Уставка по приращению угла", this->getLNRef(), false)),
      OpDlTmms(std::make_shared<ING>("Уставка по времени", this->getLNRef(), false)),
      Op(std::make_shared<ACT>("Действие на отключение", this->getLNRef(), false)),
      Str(std::make_shared<ACD>("Пуск защиты", this->getLNRef(), false)),
      BlkDPP(nullptr)
{
}

void PIOC::setStrVal(double strVal)
{
    StrVal->setMag->f->setvalue(strVal);
}

void PIOC::setStrAng(double strAngRad)
{
    StrAng->setMag->f->setvalue(strAngRad);
}

void PIOC::setOpDlTmms(double seconds)
{
    OpDlTmms->setVal->setvalue(static_cast<int32_t>(seconds * 1e3));
}

void PIOC::acceptDataFromMSQI(std::shared_ptr<CMV> data)
{
    CurrentVec = data;
}

void PIOC::acceptBlock(std::shared_ptr<ACT> blk)
{
    BlkDPP = blk;
}



double wrapAngle(double a)
{
    while (a > M_PI)  a -= 2.0 * M_PI;
    while (a < -M_PI) a += 2.0 * M_PI;
    return a;
}

static std::complex<double> rotateToSyncFrame(double mag, double ang, double tSeconds)
{
    const double w = 2.0 * M_PI * PIOC::kNominalFreqHz;
    return std::polar(mag, ang - w * tSeconds);
}



void PIOC::checkStr(int sampleIndex, double timedat)
{
    if (!CurrentVec) return;

    const double curMag = CurrentVec->cVal->getMag();
    const double curAng = CurrentVec->cVal->getAng();

    if (!curValid) {
        curValid = true;
        lastCurMag = curMag;
        lastCurAng = curAng;
        tCur = timedat;
    } else {
        const bool magChanged = std::abs(curMag - lastCurMag) > kEpsMag;
        const bool angChanged = std::abs(wrapAngle(curAng - lastCurAng)) > kEpsAng;
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
        Op->general->setvalue(false);
        wasBlocked = false;
        return;
    }

    /* Первый такт после прогрева: PrevVec ещё не инициализирован реальной величиной. */
    if (!prevValid) {
        PrevVec->cVal->setMag(curMag);
        PrevVec->cVal->setAng(curAng);
        prevValid = true;
        tPrev = tCur;
        Str->general->setvalue(false);
        Op->general->setvalue(false);
        wasBlocked = false;
        return;
    }

    const double prevMag = PrevVec->cVal->getMag();
    const double prevAng = PrevVec->cVal->getAng();

    const std::complex<double> cur = rotateToSyncFrame(curMag, curAng, tCur);
    const std::complex<double> prev = rotateToSyncFrame(prevMag, prevAng, tPrev);
    const std::complex<double> diff = cur - prev;

    const double dMag = std::abs(diff);
    const double dPhi = std::abs(wrapAngle(std::arg(cur) - std::arg(prev)));

    const double thrMag = StrVal->setMag->f->getvalue();
    const double thrAng = StrAng->setMag->f->getvalue();

    /* Угловой критерий имеет смысл только при заметном приращении. */
    const bool angExceeded = (dMag > (thrMag * 0.1)) && (dPhi > thrAng);
    const bool exceeded = (dMag > thrMag) || angExceeded;

    if (exceeded) {
        if (!Str->general->getvalue()) {
            Str->general->setvalue(true);
            tStr = timedat;
        }
        /* PrevVec сохраняем — измеряем длительность относительно начала скачка. */
    } else {
        /* Нет превышения: сброс пуска/срабатывания и обновление базы. */
        PrevVec->cVal->setMag(curMag);
        PrevVec->cVal->setAng(curAng);
        tPrev = tCur;
        Str->general->setvalue(false);
        Op->general->setvalue(false);
        wasBlocked = false;
    }
}

void PIOC::checkTimeStr(int sampleIndex, double timedat)
{
    const bool blocked = BlkDPP && BlkDPP->general->getvalue();
    if (sampleIndex < kWarmupSamples) {
        wasBlocked = blocked;
        return;
    }
    if (!Str->general->getvalue()) {
        wasBlocked = blocked;
        return;
    }
    /* При снятии блокировки во время пуска выдержка начинается заново. */
    if (wasBlocked && !blocked) {
        tStr = timedat;
        wasBlocked = false;
        return;
    }
    wasBlocked = blocked;
    if (blocked) return;

    const double elapsedMs = (timedat - tStr) * 1e3;
    const int32_t delayMs = OpDlTmms->setVal->getvalue();

    if (delayMs > 0 && elapsedMs >= delayMs && !blocked) {
        Op->general->setvalue(true);
    }
}

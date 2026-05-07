#pragma once

#include "CDC/ACT.h"
#include "GenLogicalNodeClass.h"
#include "Fourier&RMS.h"

#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <zmq.hpp>

struct PSCHWireMsg {
    uint32_t seq;
    uint8_t disc;
};


class PSCH : public GenLogicalNodeClass
{
public:
    std::shared_ptr<ACT> Blk;

    double kman = 4.0;
    double iManLast = 0.0;
    double ippLast = 0.0;
    double iopLast = 0.0;
    /** Дискрет тока манипуляции: 1 если iман > 0, иначе 0. */
    uint8_t localDisc = 0;
    uint8_t remoteDisc = 0;
    /** Битовое ИЛИ локального и удалённого дискрета (для телеметрии и логики). */
    uint8_t orDisc = 0;

    /** Мин. длительность подряд «ИЛИ==0» (оба дискрета 0), чтобы снять Blk. */
    double orZeroMinToUnblockS = 0.0;
    /** Выдержка непрерывного «ИЛИ==1» перед восстановлением Blk. */
    double orOneReblockDelayS = 0.0;

    /** Последний отправленный / принятый байт по сокету (для телеметрии). */
    uint8_t lastTxByte = 0;
    uint8_t lastRxByte = 0;
    uint32_t lastTxSeq = 0;
    uint32_t lastRxSeq = 0;
    bool rxGapDetected = false;

    PSCH(std::string LogicalNodeName_, std::string LogicalDeviceRef_);

    void setKman(double k);
    void setOrDelays(double orZeroMinToUnblockSeconds, double orOneReblockDelaySeconds);
    void setSource(Fourier& f);

    void bindLocal(zmq::context_t& ctx, const std::string& selfName);
    void connectRemote(zmq::context_t& ctx, const std::string& remoteName);

    void step(double timedat);

private:
    Fourier* fourier = nullptr;
    std::unique_ptr<zmq::socket_t> pull;
    std::unique_ptr<zmq::socket_t> push;

    bool inOrZeroStreak = false;
    double orZeroStreakStartT = 0.0;
    bool inOrOneStreak = false;
    double orOneStreakStartT = 0.0;
};

#include "PSCH.h"

#include <cmath>

constexpr const char kEndpointPrefix[] = "inproc://psch_";

PSCH::PSCH(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_),
      Blk(std::make_shared<ACT>("Блокировка от PSCH", this->getLNRef(), false))
{
    Blk->general->setvalue(true);  // по умолчанию защита заблокирована
}

void PSCH::setKman(double k) { kman = k; }

void PSCH::setOrDelays(double orZeroMinToUnblockSeconds, double orOneReblockDelaySeconds)
{
    orZeroMinToUnblockS = orZeroMinToUnblockSeconds;
    orOneReblockDelayS = orOneReblockDelaySeconds;
}

void PSCH::setSource(Fourier& f) { fourier = &f; }

void PSCH::bindLocal(zmq::context_t& ctx, const std::string& selfName)
{
    pull = std::make_unique<zmq::socket_t>(ctx, zmq::socket_type::pull);
    /* Важно: conflate/HWM=1 приводят к потере переходов 0/1 и ломают логику блокировки.
       Здесь держим достаточно большой буфер и вычитываем очередь в step(). */
    pull->set(zmq::sockopt::rcvhwm, 100000);
    pull->bind(std::string(kEndpointPrefix) + selfName);
}

void PSCH::connectRemote(zmq::context_t& ctx, const std::string& remoteName)
{
    push = std::make_unique<zmq::socket_t>(ctx, zmq::socket_type::push);
    push->set(zmq::sockopt::sndhwm, 100000);
    push->connect(std::string(kEndpointPrefix) + remoteName);
}

void PSCH::step(double timedat)
{
    if (!fourier) return;

    const double Fxa = fourier->getFx(Fourier::PhaseA);
    const double Fya = fourier->getFy(Fourier::PhaseA);
    const double Fxb = fourier->getFx(Fourier::PhaseB);
    const double Fyb = fourier->getFy(Fourier::PhaseB);
    const double Fxc = fourier->getFx(Fourier::PhaseC);
    const double Fyc = fourier->getFy(Fourier::PhaseC);

    constexpr double k = 1.0 / 3.0;
    constexpr double s32 = 0.8660254037844386;  // sqrt(3)/2

    ippLast = k * (Fxa - 0.5 * Fxb - s32 * Fyb - 0.5 * Fxc + s32 * Fyc);
    iopLast = k * (Fxa - 0.5 * Fxb + s32 * Fyb - 0.5 * Fxc - s32 * Fyc);
    iManLast = ippLast + kman * iopLast;

    localDisc = (iManLast > 0.0) ? 1 : 0;

    if (push) {
        PSCHWireMsg msg{++lastTxSeq, localDisc};
        lastTxByte = msg.disc;
        try {
            push->send(zmq::buffer(&msg, sizeof(msg)), zmq::send_flags::none);
        } catch (const zmq::error_t&) {
        }
    }

    /* Один принятый кадр на такт: потоки IED синхронизированы барьером, peer
       отправляет ровно одно сообщение за тот же индекс выборки (тот же t). */
    if (pull) {
        zmq::message_t zmsg;
        rxGapDetected = false;
        const uint32_t prevSeq = lastRxSeq;
        const auto res = pull->recv(zmsg, zmq::recv_flags::none);
        if (res && zmsg.size() == sizeof(PSCHWireMsg)) {
            const auto* wm = static_cast<const PSCHWireMsg*>(zmsg.data());
            lastRxSeq = wm->seq;
            lastRxByte = wm->disc;
            remoteDisc = (lastRxByte != 0) ? 1 : 0;
            if (prevSeq != 0 && lastRxSeq != (prevSeq + 1)) {
                rxGapDetected = true;
            }
        } else if (res && zmsg.size() >= 1) {
            /* backward compatibility: old 1-byte payload */
            lastRxSeq = 0;
            lastRxByte = *static_cast<const uint8_t*>(zmsg.data());
            remoteDisc = (lastRxByte != 0) ? 1 : 0;
        }
    }

    orDisc = static_cast<uint8_t>((localDisc != 0 ? 1u : 0u) | (remoteDisc != 0 ? 1u : 0u));

    /* Логика по битовому ИЛИ: (local|remote)==0 только если оба 0 — «промежуток нулей» в OR.
       Длинная непрерывная нулевая фаза OR → снять Blk; устойчивое ИЛИ==1 → восстановить Blk. */
    if (rxGapDetected) {
        Blk->general->setvalue(true);
        inOrZeroStreak = false;
        inOrOneStreak = false;
        return;
    }

    const bool orIsZero = (orDisc == 0);

    if (orIsZero) {
        inOrOneStreak = false;
        if (!inOrZeroStreak) {
            inOrZeroStreak = true;
            orZeroStreakStartT = timedat;
        }
        const double orZeroDur = timedat - orZeroStreakStartT;
        if (orZeroDur >= orZeroMinToUnblockS) {
            Blk->general->setvalue(false);
        }
    } else {
        inOrZeroStreak = false;
        if (!inOrOneStreak) {
            inOrOneStreak = true;
            orOneStreakStartT = timedat;
        }
        const double orOneDur = timedat - orOneStreakStartT;
        if (orOneDur >= orOneReblockDelayS) {
            Blk->general->setvalue(true);
        }
    }
}

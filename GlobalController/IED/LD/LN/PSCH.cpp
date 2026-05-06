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

void PSCH::setIManThr(double thr) { iManThr = thr; }

void PSCH::setSource(Fourier& f) { fourier = &f; }

void PSCH::bindLocal(zmq::context_t& ctx, const std::string& selfName)
{
    pull = std::make_unique<zmq::socket_t>(ctx, zmq::socket_type::pull);
    int hwm = 1;
    pull->set(zmq::sockopt::rcvhwm, hwm);
    pull->set(zmq::sockopt::conflate, 1);
    pull->bind(std::string(kEndpointPrefix) + selfName);
}

void PSCH::connectRemote(zmq::context_t& ctx, const std::string& remoteName)
{
    push = std::make_unique<zmq::socket_t>(ctx, zmq::socket_type::push);
    int hwm = 1;
    push->set(zmq::sockopt::sndhwm, hwm);
    push->set(zmq::sockopt::conflate, 1);
    push->connect(std::string(kEndpointPrefix) + remoteName);
}

void PSCH::step()
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

    localActive = (iManLast > iManThr);

    if (push) {
        const uint8_t flag = localActive ? 1 : 0;
        lastTxByte = flag;
        try {
            push->send(zmq::buffer(&flag, sizeof(flag)),
                       zmq::send_flags::dontwait);
        } catch (const zmq::error_t&) {
        }
    }

    if (pull) {
        zmq::message_t msg;
        while (true) {
            auto res = pull->recv(msg, zmq::recv_flags::dontwait);
            if (!res) break;
            if (msg.size() >= 1) {
                lastRxByte = *static_cast<const uint8_t*>(msg.data());
                remoteActive = (lastRxByte != 0);
            }
        }
    }

    Blk->general->setvalue(!(localActive && remoteActive));
}

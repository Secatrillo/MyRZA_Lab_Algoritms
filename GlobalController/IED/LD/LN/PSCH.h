#pragma once

#include "CDC/ACT.h"
#include "GenLogicalNodeClass.h"
#include "Fourier&RMS.h"

#include <cstdint>
#include <memory>
#include <string>
#include <zmq.hpp>


class PSCH : public GenLogicalNodeClass
{
public:
    std::shared_ptr<ACT> Blk;

    double kman = 4.0;
    /** Уставка по току манипуляции: активность при iман > IManThr. */
    double iManThr = 0.0;
    double iManLast = 0.0;
    double ippLast = 0.0;
    double iopLast = 0.0;
    bool   localActive = false;
    bool   remoteActive = false;

    /** Последний отправленный / принятый байт по сокету (для телеметрии). */
    uint8_t lastTxByte = 0;
    uint8_t lastRxByte = 0;

    PSCH(std::string LogicalNodeName_, std::string LogicalDeviceRef_);

    void setKman(double k);
    void setIManThr(double thr);
    void setSource(Fourier& f);

    void bindLocal(zmq::context_t& ctx, const std::string& selfName);
    void connectRemote(zmq::context_t& ctx, const std::string& remoteName);

    void step();

private:
    Fourier* fourier = nullptr;
    std::unique_ptr<zmq::socket_t> pull;
    std::unique_ptr<zmq::socket_t> push;
};

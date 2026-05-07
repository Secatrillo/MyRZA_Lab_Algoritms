#pragma once

#include "IedStepBarrier.h"
#include "LD/PROT.h"
#include "LD/CTRL.h"
#include "LD/MEAS.h"
#include "../../include.h"
#include <IEDSettings.h>
#include "IEDTelemetry.h"
#include <ParserComtrade.h>
#include <memory>
#include <string>
#include <vector>

class IED {
public:
    IED(std::string IEDName_);

    void setSettings(std::shared_ptr<Settings> settings);

    void bindPSCHLocal();
    void connectPSCHRemote();

    void IEDInitDataTransfer();
    void modelIEDWork(int& counter);
    /** Синхронизация тактов между IED (одинаковый индекс выборки и t для PSCH). */
    void setSimTickBarrier(std::shared_ptr<IedStepBarrier> b) { simTickBarrier = std::move(b); }

private:
    void sendTelemetrySample(int sampleIndex);

    /** Collects all channels for this tick; extend by appending graphs/lines here. */
    IEDTelemetryFrame buildTelemetryFrame(int sampleIndex) const;

    std::string IEDName;
    std::string remoteName;
    MEAS meas;
    PROT prot;
    CTRL ctrl;
    std::shared_ptr<zmq::context_t> context;
    std::shared_ptr<ParserComtrade> parser;
    std::unique_ptr<zmq::socket_t> telemetryPush;
    std::shared_ptr<IedStepBarrier> simTickBarrier;
};

#pragma once

#include "LD/PROT.h"
#include "LD/CTRL.h"
#include "LD/MEAS.h"
#include "../../include.h"
#include <IEDSettings.h>
#include "IEDTelemetry.h"
#include <ParserComtrade.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class IED {
public:
    explicit IED(std::string IEDName_);

    void setSettings(std::shared_ptr<Settings> settings);

    void IEDInitDataTransfer();
    void modelIEDWork(int& counter);

private:
    void sendTelemetrySample(int sampleIndex);

    IEDTelemetryFrame buildTelemetryFrame(int sampleIndex);

    std::string IEDName;
    MEAS meas;
    PROT prot;
    CTRL ctrl;
    std::shared_ptr<zmq::context_t> context;
    std::shared_ptr<ParserComtrade> parser;
    std::unique_ptr<zmq::socket_t> telemetryPush;

    /** Полигоны зон PDIS в JSON — ровно один раз за процесс (объём сообщения). */
    bool telemetryZonePolygonsSentOnce_{false};
};

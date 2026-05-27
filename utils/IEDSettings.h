#pragma once

#include <zmq.hpp>
#include <ParserComtrade.h>
#include <memory>
#include <string>

/**
 * Уставки зоны срабатывания одной ступени PDIS (соответствуют setStrVal* / setOpDlTmms в PDIS).
 * strValFiRad — угол максимальной чувствительности в радианах.
 */
struct PdisPickupSettings {
    double opDlTmms = 0.05;       /* с, выдержка времени срабатывания Op */
    double strValR = 12.0;      /* Ом, полуось эллипса вдоль φ */
    double strValX = 12.0;      /* Ом, полуось ⊥ φ */
    double strValFiRad = 0.0;   /* рад */
    double strValOffset = 0.0;  /* Ом, смещение центра вдоль φ */
    double strValHyst = 1.05;   /* множитель зоны возврата относительно pickup */
};

/** Настройки одного IED: два COMTRADE, Фурье, RDIS, три набора уставок PDIS, ZMQ-контекст. */
class Settings {
public:
    Settings(double posStrVal, double posStrAng,
             double negStrVal, double negStrAng,
             bool fourierMode,
             int discrit,
             std::shared_ptr<ParserComtrade> parserCurrent,
             std::shared_ptr<ParserComtrade> parserVoltage,
             std::shared_ptr<zmq::context_t> context,
             PdisPickupSettings pdis1,
             PdisPickupSettings pdis2,
             PdisPickupSettings pdis3)
        : posStrVal(posStrVal),
          posStrAng(posStrAng),
          negStrVal(negStrVal),
          negStrAng(negStrAng),
          fourierMode(fourierMode),
          discrit(discrit),
          parserCurrent(std::move(parserCurrent)),
          parserVoltage(std::move(parserVoltage)),
          context(std::move(context)),
          pdis1_(std::move(pdis1)),
          pdis2_(std::move(pdis2)),
          pdis3_(std::move(pdis3))
    {}

    double getPosStrVal() const { return posStrVal; }
    double getPosStrAng() const { return posStrAng; }
    double getNegStrVal() const { return negStrVal; }
    double getNegStrAng() const { return negStrAng; }

    bool getFourierMode() const { return fourierMode; }
    int getDiscrit() const { return discrit; }
    std::shared_ptr<ParserComtrade> getParser() const { return parserCurrent; }
    std::shared_ptr<ParserComtrade> getParserCurrent() const { return parserCurrent; }
    std::shared_ptr<ParserComtrade> getParserVoltage() const { return parserVoltage; }
    std::shared_ptr<zmq::context_t> getContext() const { return context; }

    const PdisPickupSettings& getPdisPickup1() const { return pdis1_; }
    const PdisPickupSettings& getPdisPickup2() const { return pdis2_; }
    const PdisPickupSettings& getPdisPickup3() const { return pdis3_; }

private:
    double posStrVal;
    double posStrAng;
    double negStrVal;
    double negStrAng;
    bool fourierMode;
    int discrit;
    std::shared_ptr<ParserComtrade> parserCurrent;
    std::shared_ptr<ParserComtrade> parserVoltage;
    std::shared_ptr<zmq::context_t> context;
    PdisPickupSettings pdis1_;
    PdisPickupSettings pdis2_;
    PdisPickupSettings pdis3_;
};

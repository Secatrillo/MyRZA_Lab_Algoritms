#pragma once

#include <zmq.hpp>
#include <ParserComtrade.h>
#include <memory>
#include <string>

class Settings {
public:
    Settings(double posStrVal, double posStrAng, double posTimeS,
             double negStrVal, double negStrAng, double negTimeS,
             double kman,
             double iManThr,
             bool fourierMode,
             int discrit,
             std::shared_ptr<ParserComtrade> parser,
             std::shared_ptr<zmq::context_t> context,
             std::string remoteName)
        : posStrVal(posStrVal),
          posStrAng(posStrAng),
          posTimeS(posTimeS),
          negStrVal(negStrVal),
          negStrAng(negStrAng),
          negTimeS(negTimeS),
          kman(kman),
          iManThr(iManThr),
          fourierMode(fourierMode),
          discrit(discrit),
          parser(std::move(parser)),
          context(std::move(context)),
          remoteName(std::move(remoteName))
    {}

    double getPosStrVal() const { return posStrVal; }
    double getPosStrAng() const { return posStrAng; }
    double getPosTimeS() const { return posTimeS; }
    double getNegStrVal() const { return negStrVal; }
    double getNegStrAng() const { return negStrAng; }
    double getNegTimeS() const { return negTimeS; }
    double getKman() const { return kman; }
    double getIManThr() const { return iManThr; }

    bool getFourierMode() const { return fourierMode; }
    int  getDiscrit() const { return discrit; }
    std::shared_ptr<ParserComtrade> getParser() const { return parser; }
    std::shared_ptr<zmq::context_t> getContext() const { return context; }
    const std::string& getRemoteName() const { return remoteName; }

private:
    double posStrVal;
    double posStrAng;
    double posTimeS;
    double negStrVal;
    double negStrAng;
    double negTimeS;
    double kman;
    double iManThr;
    bool fourierMode;
    int discrit;
    std::shared_ptr<ParserComtrade> parser;
    std::shared_ptr<zmq::context_t> context;
    std::string remoteName;
};

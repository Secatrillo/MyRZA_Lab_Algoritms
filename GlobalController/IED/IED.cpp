#include "IED.h"



double discrete(double x)
{
    return (x != 0.0) ? 1.0 : 0.0;
}



IED::IED(std::string IEDName_)
    : IEDName(std::move(IEDName_)),
      meas("MEAS"),
      prot("PROT"),
      ctrl("CTRL")
{
}

void IED::setSettings(std::shared_ptr<Settings> settings)
{
    prot.setSettings(settings->getPosStrVal(), settings->getPosStrAng(), settings->getPosTimeS(),
                     settings->getNegStrVal(), settings->getNegStrAng(), settings->getNegTimeS(),
                     settings->getKman(),
                     settings->getIManThr(),
                     settings->getParser());
    meas.setSettings(settings->getFourierMode(), settings->getDiscrit(), settings->getParser());
    context = settings->getContext();
    parser = settings->getParser();
    remoteName = settings->getRemoteName();
}

void IED::bindPSCHLocal()
{
    if (!context) return;
    prot.bindPSCHLocal(*context, IEDName);
}

void IED::connectPSCHRemote()
{
    if (!context) return;
    prot.connectPSCHRemote(*context, remoteName);
}

void IED::IEDInitDataTransfer()
{
    meas.sendLSVSDataToMMXU();
    meas.sendFourierDataToMMXU();
    meas.sendMMXUDataToMSQI();
    prot.acceptDataFromMSQI(meas.MSQI1.PositiveSeq, meas.MSQI1.NegativeSeq);
    prot.linkFourierToPSCH(meas.Fourier1);
    prot.linkPSCHToPIOC();
    ctrl.receiveTripSignal(prot.PTRC1.Tr);
}

IEDTelemetryFrame IED::buildTelemetryFrame(int sampleIndex) const
{
    IEDTelemetryFrame frame;
    frame.name = IEDName;
    if (parser) {
        const auto& times = parser->getTimeData();
        if (sampleIndex >= 0 && static_cast<size_t>(sampleIndex) < times.size())
            frame.t = times[static_cast<size_t>(sampleIndex)];
    }

    // --- Instantaneous phase currents (SV)
    {
        TelemetryGraphBlock g;
        g.graph_name = "Instant currents";
        g.plot_style = "line";
        g.lines.push_back({"Ia_inst", meas.LSVS1.currentA->getInstMag()});
        g.lines.push_back({"Ib_inst", meas.LSVS1.currentB->getInstMag()});
        g.lines.push_back({"Ic_inst", meas.LSVS1.currentC->getInstMag()});
        frame.graphs.push_back(std::move(g));
    }

    // --- Fourier magnitudes (MMXU)
    {
        TelemetryGraphBlock g;
        g.graph_name = "Fourier RMS";
        g.plot_style = "line";
        g.lines.push_back({"Ia_fourier", meas.MMXU1.A->phsA->cVal->getMag()});
        g.lines.push_back({"Ib_fourier", meas.MMXU1.A->phsB->cVal->getMag()});
        g.lines.push_back({"Ic_fourier", meas.MMXU1.A->phsC->cVal->getMag()});
        frame.graphs.push_back(std::move(g));
    }

    // --- Symmetrical components
    {
        TelemetryGraphBlock g;
        g.graph_name = "Sequence components";
        g.plot_style = "line";
        g.lines.push_back({"I_positive", meas.MSQI1.PositiveSeq->cVal->getMag()});
        g.lines.push_back({"I_negative", meas.MSQI1.NegativeSeq->cVal->getMag()});
        g.lines.push_back({"I_zero", meas.MSQI1.ZeroSeq->cVal->getMag()});
        frame.graphs.push_back(std::move(g));
    }

    // --- PIOC1 (positive seq increment)
    {
        TelemetryGraphBlock g;
        g.graph_name = "PIOC1 (PositiveSeq)";
        g.plot_style = "step";
        g.lines.push_back({"Str", discrete(prot.PIOC1.Str->general->getvalue())});
        g.lines.push_back({"Op",  discrete(prot.PIOC1.Op->general->getvalue())});
        g.lines.push_back({"Blc",  discrete(prot.PIOC1.BlkDPP->general->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    // --- PIOC2 (negative seq increment)
    {
        TelemetryGraphBlock g;
        g.graph_name = "PIOC2 (NegativeSeq)";
        g.plot_style = "step";
        g.lines.push_back({"Str", discrete(prot.PIOC2.Str->general->getvalue())});
        g.lines.push_back({"Op",  discrete(prot.PIOC2.Op->general->getvalue())});
        g.lines.push_back({"Blc",  discrete(prot.PIOC2.BlkDPP->general->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    // --- PSCH: ток манипуляции и составляющие (аналоговые)
    {
        TelemetryGraphBlock g;
        g.graph_name = "PSCH currents";
        g.plot_style = "line";
        g.lines.push_back({"i_pp", prot.PSCH1.ippLast});
        g.lines.push_back({"i_op", prot.PSCH1.iopLast});
        g.lines.push_back({"i_man", prot.PSCH1.iManLast});
        g.lines.push_back({"i_man_thr", prot.PSCH1.iManThr});  // уставка (константа на графике)
        frame.graphs.push_back(std::move(g));
    }

    // --- PSCH: обмен по сокетам и блокировка (дискретные)
    {
        TelemetryGraphBlock g;
        g.graph_name = "PSCH link";
        g.plot_style = "step";
        g.lines.push_back({"tx_to_remote", discrete(static_cast<double>(prot.PSCH1.lastTxByte))});
        g.lines.push_back({"rx_from_remote", discrete(static_cast<double>(prot.PSCH1.lastRxByte))});
        g.lines.push_back({"Blk", discrete(prot.PSCH1.Blk->general->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    // --- Trip + breaker
    {
        TelemetryGraphBlock g;
        g.graph_name = "Trip and breaker";
        g.plot_style = "step";
        g.lines.push_back({"PTRC_Trip", discrete(prot.PTRC1.Tr->general->getvalue())});
        g.lines.push_back({"XCBR_Pos",  discrete(ctrl.XCBR1.Pos->stVal->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    return frame;
}

void IED::sendTelemetrySample(int sampleIndex)
{
    if (!context)
        return;
    if (!telemetryPush) {
        telemetryPush = std::make_unique<zmq::socket_t>(*context, zmq::socket_type::push);
        telemetryPush->connect(kIedTelemetryInproc);
    }
    const std::string payload = serializeIEDTelemetryFrame(buildTelemetryFrame(sampleIndex));
    telemetryPush->send(zmq::buffer(payload), zmq::send_flags::none);
}

void IED::modelIEDWork(int& counter)
{
    for (int i = 0; i < counter; i++) {
        meas.acceptSV(i);

        meas.calculateFourier();
        meas.sendFourierDataToMMXU();
        meas.sendMMXUDataToMSQI();
        meas.calculateSequenses();

        prot.imitateRP(i);

        ctrl.actOnSignal();

        sendTelemetrySample(i);
    }
}

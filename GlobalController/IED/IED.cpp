#include "IED.h"

#include <algorithm>
#include <cmath>
#include <thread>

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
    prot.setSettings(settings->getPosStrVal(), settings->getPosStrAng(),
                     settings->getNegStrVal(), settings->getNegStrAng(),
                     settings->getParser(),
                     settings->getPdisPickup1(), settings->getPdisPickup2(), settings->getPdisPickup3());
    meas.setSettings(settings->getFourierMode(), settings->getDiscrit(), settings->getParserCurrent(),
                     settings->getParserVoltage());
    context = settings->getContext();
    parser = settings->getParser();
}

void IED::IEDInitDataTransfer()
{
    meas.calculateFourier_V2();
    meas.sendFourierDataToMMXU_V2();
    meas.sendMMXU_V2_DataToMSQI();
    prot.acceptDataFromMSQI(meas.MSQI1.PositiveSeq, meas.MSQI1.NegativeSeq);
    prot.linkDistanceProtection(meas.mmxuV2.CurZ);
    ctrl.receiveTripSignal(prot.PTRC1.Tr);
}

IEDTelemetryFrame IED::buildTelemetryFrame(int sampleIndex)
{
    IEDTelemetryFrame frame;
    frame.name = IEDName;
    if (parser) {
        const auto& times = parser->getTimeData();
        if (sampleIndex >= 0 && static_cast<size_t>(sampleIndex) < times.size())
            frame.t = times[static_cast<size_t>(sampleIndex)];
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "Instant currents (LSVS_I)";
        g.plot_style = "line";
        g.lines.push_back({"Ia_inst", meas.LSVS_I.currentA->getInstMag()});
        g.lines.push_back({"Ib_inst", meas.LSVS_I.currentB->getInstMag()});
        g.lines.push_back({"Ic_inst", meas.LSVS_I.currentC->getInstMag()});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "Instant line voltages (LSVS_U)";
        g.plot_style = "line";
        g.lines.push_back({"Uab_inst", meas.LSVS_U.currentA->getInstMag()});
        g.lines.push_back({"Ubc_inst", meas.LSVS_U.currentB->getInstMag()});
        g.lines.push_back({"Uca_inst", meas.LSVS_U.currentC->getInstMag()});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "MMXU_V2 currents (RMS)";
        g.plot_style = "line";
        g.lines.push_back({"Ia_v2", meas.mmxuV2.A->phsA->cVal->getMag()});
        g.lines.push_back({"Ib_v2", meas.mmxuV2.A->phsB->cVal->getMag()});
        g.lines.push_back({"Ic_v2", meas.mmxuV2.A->phsC->cVal->getMag()});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "MMXU_V2 phase voltages (RMS)";
        g.plot_style = "line";
        g.lines.push_back({"Ua_v2", meas.mmxuV2.PhV->phsA->cVal->getMag()});
        g.lines.push_back({"Ub_v2", meas.mmxuV2.PhV->phsB->cVal->getMag()});
        g.lines.push_back({"Uc_v2", meas.mmxuV2.PhV->phsC->cVal->getMag()});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "Sequence components (MSQI from Fourier_I)";
        g.plot_style = "line";
        g.lines.push_back({"I_positive", meas.MSQI1.PositiveSeq->cVal->getMag()});
        g.lines.push_back({"I_negative", meas.MSQI1.NegativeSeq->cVal->getMag()});
        g.lines.push_back({"I_zero", meas.MSQI1.ZeroSeq->cVal->getMag()});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "RDIS / PTRC_BLK";
        g.plot_style = "step";
        g.lines.push_back({"RDIS_P_Blk", discrete(prot.RDIS_P.Blk->general->getvalue())});
        g.lines.push_back({"RDIS_N_Blk", discrete(prot.RDIS_N.Blk->general->getvalue())});
        g.lines.push_back({"BlkCombined", discrete(prot.BlkSvgCombined->general->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "PDIS_1";
        g.plot_style = "step";
        g.lines.push_back({"PDIS1_Str", discrete(prot.PDIS1.Str->general->getvalue())});
        g.lines.push_back({"PDIS1_Op", discrete(prot.PDIS1.Op->general->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "PDIS_2";
        g.plot_style = "step";
        g.lines.push_back({"PDIS2_Str", discrete(prot.PDIS2.Str->general->getvalue())});
        g.lines.push_back({"PDIS2_Op", discrete(prot.PDIS2.Op->general->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "PDIS_3";
        g.plot_style = "step";
        g.lines.push_back({"PDIS3_Str", discrete(prot.PDIS3.Str->general->getvalue())});
        g.lines.push_back({"PDIS3_Op", discrete(prot.PDIS3.Op->general->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    {
        TelemetryGraphBlock g;
        g.graph_name = "Trip and breaker";
        g.plot_style = "step";
        g.lines.push_back({"PTRC_Trip", discrete(prot.PTRC1.Tr->general->getvalue())});
        g.lines.push_back({"XCBR_Pos", discrete(ctrl.XCBR1.Pos->stVal->getvalue())});
        frame.graphs.push_back(std::move(g));
    }

    const uint64_t mxRev = std::max({prot.PDIS1.getZoneRevision(), prot.PDIS2.getZoneRevision(), prot.PDIS3.getZoneRevision()});

    IEDRxView rx;
    if (meas.mmxuV2.CurZ) {
        if (meas.mmxuV2.CurZ->phsA && meas.mmxuV2.CurZ->phsA->cVal) {
            const double m = meas.mmxuV2.CurZ->phsA->cVal->getMag();
            const double a = meas.mmxuV2.CurZ->phsA->cVal->getAng();
            rx.z_ab.point_r = m * std::cos(a);
            rx.z_ab.point_x = m * std::sin(a);
        }
        if (meas.mmxuV2.CurZ->phsB && meas.mmxuV2.CurZ->phsB->cVal) {
            const double m = meas.mmxuV2.CurZ->phsB->cVal->getMag();
            const double a = meas.mmxuV2.CurZ->phsB->cVal->getAng();
            rx.z_bc.point_r = m * std::cos(a);
            rx.z_bc.point_x = m * std::sin(a);
        }
        if (meas.mmxuV2.CurZ->phsC && meas.mmxuV2.CurZ->phsC->cVal) {
            const double m = meas.mmxuV2.CurZ->phsC->cVal->getMag();
            const double a = meas.mmxuV2.CurZ->phsC->cVal->getAng();
            rx.z_ca.point_r = m * std::cos(a);
            rx.z_ca.point_x = m * std::sin(a);
        }
    }
    rx.max_zone_revision = mxRev;

    if (!telemetryZonePolygonsSentOnce_) {
        std::vector<TelemetryRxZone> zones;
        zones.reserve(3);
        for (auto* pdis : {&prot.PDIS1, &prot.PDIS2, &prot.PDIS3}) {
            TelemetryRxZone z;
            z.name = pdis->getLogicalNodeName();
            z.revision = pdis->getZoneRevision();
            pdis->exportPickupPolygon(z.pickup);
            pdis->exportReturnPolygon(z.ret);
            zones.push_back(std::move(z));
        }
        rx.zones = std::move(zones);
        telemetryZonePolygonsSentOnce_ = true;
    } else {
        rx.zones = std::nullopt;
    }
    frame.rx_view = std::move(rx);

    return frame;
}

void IED::sendTelemetrySample(int sampleIndex)
{
    if (!context)
        return;
    if (!telemetryPush) {
        telemetryPush = std::make_unique<zmq::socket_t>(*context, zmq::socket_type::push);
        /* Без лимита иначе при быстрой симуляции очередь переполняется (по умолчанию ~1000) и телеметрия теряется. */
        telemetryPush->set(zmq::sockopt::sndhwm, 0);
        telemetryPush->set(zmq::sockopt::linger, 0);
        telemetryPush->connect(kIedTelemetryInproc);
    }
    const std::string payload = serializeIEDTelemetryFrame(buildTelemetryFrame(sampleIndex));
    telemetryPush->send(zmq::buffer(payload), zmq::send_flags::none);
}

void IED::modelIEDWork(int& counter)
{
    for (int i = 0; i < counter; i++) {
        meas.acceptSV(i);

        meas.calculateFourier_V2();
        meas.sendFourierDataToMMXU_V2();
        meas.sendMMXU_V2_DataToMSQI();
        meas.calculateSequenses();

        prot.imitateRP(i);

        ctrl.actOnSignal();

        sendTelemetrySample(i);

        if ((i & 16383) == 0)
            std::this_thread::yield();
    }
}

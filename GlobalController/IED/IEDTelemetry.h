#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

/** One scalar sample on a graph (extend by adding more lines / graphs on the server). */
struct TelemetryLineSample {
    std::string line_name;
    double value;
};

/**
 * One subplot worth of data. plot_style: "line" (default) or "step" for discrete signals.
 * To add a new graph, push another TelemetryGraphBlock in IED::fillTelemetryFrame().
 */
struct TelemetryGraphBlock {
    std::string graph_name;
    std::string plot_style;  // "line" | "step"
    std::vector<TelemetryLineSample> lines;
};

struct TelemetryRxZone {
    std::string name;
    uint64_t revision = 0;
    std::vector<std::pair<double, double>> pickup;
    std::vector<std::pair<double, double>> ret;
};

/** Одна точка R–X на шаге (pr/px в JSON); траектории строит только клиент телеметрии. */
struct IEDRxTrace {
    double point_r = 0.0;
    double point_x = 0.0;
};

struct IEDRxView {
    IEDRxTrace z_ab;
    IEDRxTrace z_bc;
    IEDRxTrace z_ca;
    uint64_t max_zone_revision = 0;
    /** Полигоны зон: только в первом сообщении процесса (см. IED::telemetryZonePolygonsSentOnce_). */
    std::optional<std::vector<TelemetryRxZone>> zones;
};

/** One simulation tick for one IED — matches the JSON shape expected by Monitoring.py */
struct IEDTelemetryFrame {
    std::string name;
    double t = 0.0;
    std::vector<TelemetryGraphBlock> graphs;
    std::optional<IEDRxView> rx_view;
};

/** Minimal JSON serializer (no third-party JSON library). */
std::string serializeIEDTelemetryFrame(const IEDTelemetryFrame& frame);

/** inproc endpoint: Python binds PULL, each IED connects PUSH (same context). */
inline constexpr const char kIedTelemetryInproc[] = "inproc://ied_telemetry";

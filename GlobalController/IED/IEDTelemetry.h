#pragma once

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

/** One simulation tick for one IED — matches the JSON shape expected by Monitoring.py */
struct IEDTelemetryFrame {
    std::string name;
    double t = 0.0;
    std::vector<TelemetryGraphBlock> graphs;
};

/** Minimal JSON serializer (no third-party JSON library). */
std::string serializeIEDTelemetryFrame(const IEDTelemetryFrame& frame);

/** inproc endpoint: Python binds PULL, each IED connects PUSH (same context). */
inline constexpr const char kIedTelemetryInproc[] = "inproc://ied_telemetry";

#include "IEDTelemetry.h"

#include <iomanip>
#include <sstream>
#include <string_view>

static std::string jsonEscape(std::string_view s)
{
    std::string o;
    o.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
        case '\\':
            o += "\\\\";
            break;
        case '"':
            o += "\\\"";
            break;
        case '\b':
            o += "\\b";
            break;
        case '\f':
            o += "\\f";
            break;
        case '\n':
            o += "\\n";
            break;
        case '\r':
            o += "\\r";
            break;
        case '\t':
            o += "\\t";
            break;
        default:
            if (c < 0x20) {
                std::ostringstream hex;
                hex << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                o += hex.str();
            } else {
                o += static_cast<char>(c);
            }
        }
    }
    return o;
}

static std::string formatDouble(double v)
{
    std::ostringstream oss;
    oss << std::setprecision(15) << v;
    return oss.str();
}

std::string serializeIEDTelemetryFrame(const IEDTelemetryFrame& frame)
{
    std::ostringstream json;
    json << '{';
    json << "\"name\":\"" << jsonEscape(frame.name) << "\",";
    json << "\"t\":" << formatDouble(frame.t) << ',';
    json << "\"graphs\":[";
    for (size_t gi = 0; gi < frame.graphs.size(); ++gi) {
        const auto& g = frame.graphs[gi];
        if (gi)
            json << ',';
        json << '{';
        json << "\"graph_name\":\"" << jsonEscape(g.graph_name) << "\",";
        json << "\"plot_style\":\"" << jsonEscape(g.plot_style) << "\",";
        json << "\"lines\":[";
        for (size_t li = 0; li < g.lines.size(); ++li) {
            const auto& ln = g.lines[li];
            if (li)
                json << ',';
            json << '{';
            json << "\"line_name\":\"" << jsonEscape(ln.line_name) << "\",";
            json << "\"value\":" << formatDouble(ln.value);
            json << '}';
        }
        json << ']';
        json << '}';
    }
    json << ']';
    json << '}';
    return json.str();
}

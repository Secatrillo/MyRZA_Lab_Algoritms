#include "IEDTelemetry.h"

#include <charconv>
#include <cstdio>
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

static void appendJsonDouble(std::string& out, double v)
{
    char buf[64];
    const auto r = std::to_chars(buf, buf + sizeof(buf), v);
    if (r.ec == std::errc()) {
        out.append(buf, r.ptr);
        return;
    }
    char fb[64];
    std::snprintf(fb, sizeof(fb), "%.15g", v);
    out += fb;
}

static void appendDoubleArray(std::string& json, const std::vector<double>& a)
{
    json.push_back('[');
    for (size_t i = 0; i < a.size(); ++i) {
        if (i)
            json.push_back(',');
        appendJsonDouble(json, a[i]);
    }
    json.push_back(']');
}

static void appendPointArray(std::string& json, const std::vector<std::pair<double, double>>& pts)
{
    json.push_back('[');
    for (size_t i = 0; i < pts.size(); ++i) {
        if (i)
            json.push_back(',');
        json.push_back('[');
        appendJsonDouble(json, pts[i].first);
        json.push_back(',');
        appendJsonDouble(json, pts[i].second);
        json.push_back(']');
    }
    json.push_back(']');
}

static void appendTraceBlock(std::string& json, std::string_view key, const IEDRxTrace& tr)
{
    json.push_back('"');
    json.append(key);
    /* Компактные ключи pr/px — только точка текущего шага; траектория накапливается в Python. */
    json.append("\":{\"pr\":");
    appendJsonDouble(json, tr.point_r);
    json.append(",\"px\":");
    appendJsonDouble(json, tr.point_x);
    json.push_back('}');
}

static void appendRxView(std::string& json, const IEDRxView& rx)
{
    json.append("\"rx_view\":{");
    appendTraceBlock(json, "z_ab", rx.z_ab);
    json.push_back(',');
    appendTraceBlock(json, "z_bc", rx.z_bc);
    json.push_back(',');
    appendTraceBlock(json, "z_ca", rx.z_ca);
    json.append(",\"max_zone_revision\":");
    json.append(std::to_string(rx.max_zone_revision));
    if (rx.zones.has_value()) {
        json.append(",\"zones\":[");
        const auto& zones = *rx.zones;
        for (size_t zi = 0; zi < zones.size(); ++zi) {
            const auto& z = zones[zi];
            if (zi)
                json.push_back(',');
            json.push_back('{');
            json.append("\"name\":\"");
            json.append(jsonEscape(z.name));
            json.append("\",");
            json.append("\"revision\":");
            json.append(std::to_string(z.revision));
            json.append(",\"pickup\":");
            appendPointArray(json, z.pickup);
            json.append(",\"return_poly\":");
            appendPointArray(json, z.ret);
            json.push_back('}');
        }
        json.push_back(']');
    }
    json.push_back('}');
}

std::string serializeIEDTelemetryFrame(const IEDTelemetryFrame& frame)
{
    size_t reserve = 2048;
    for (const auto& g : frame.graphs) {
        reserve += g.graph_name.size() + g.plot_style.size() + g.lines.size() * 48 + 64;
    }
    if (frame.rx_view.has_value()) {
        const IEDRxView& rx = *frame.rx_view;
        reserve += 256;
        if (rx.zones.has_value()) {
            for (const auto& z : *rx.zones) {
                reserve += z.pickup.size() * 40 + z.ret.size() * 40 + 128;
            }
        }
    }

    std::string json;
    json.reserve(reserve);
    json.push_back('{');
    json.append("\"name\":\"");
    json.append(jsonEscape(frame.name));
    json.append("\",");
    json.append("\"t\":");
    appendJsonDouble(json, frame.t);
    json.append(",\"graphs\":[");
    for (size_t gi = 0; gi < frame.graphs.size(); ++gi) {
        const auto& g = frame.graphs[gi];
        if (gi)
            json.push_back(',');
        json.push_back('{');
        json.append("\"graph_name\":\"");
        json.append(jsonEscape(g.graph_name));
        json.append("\",");
        json.append("\"plot_style\":\"");
        json.append(jsonEscape(g.plot_style));
        json.append("\",");
        json.append("\"lines\":[");
        for (size_t li = 0; li < g.lines.size(); ++li) {
            const auto& ln = g.lines[li];
            if (li)
                json.push_back(',');
            json.push_back('{');
            json.append("\"line_name\":\"");
            json.append(jsonEscape(ln.line_name));
            json.append("\",");
            json.append("\"value\":");
            appendJsonDouble(json, ln.value);
            json.push_back('}');
        }
        json.append("]}");
    }
    json.push_back(']');
    if (frame.rx_view.has_value()) {
        json.push_back(',');
        appendRxView(json, *frame.rx_view);
    }
    json.push_back('}');
    return json;
}

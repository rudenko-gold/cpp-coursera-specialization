#include "transport_catalog.h"

#include <sstream>
#include <iomanip>

using namespace std;

TransportCatalog::TransportCatalog(vector<Descriptions::InputQuery> data, const Json::Dict& routing_settings_json, const Json::Dict& render_settings_json) : render_settings(render_settings_json) {
    auto stops_end = partition(begin(data), end(data), [](const auto& item) {
        return holds_alternative<Descriptions::Stop>(item);
    });

    Descriptions::StopsDict stops_dict;
    for (const auto& item : Range{begin(data), stops_end}) {
        const auto& stop = get<Descriptions::Stop>(item);
        stops_dict[stop.name] = &stop;
        stops_.insert({ stop.name, {{}, stop.position } });
    }

    Descriptions::BusesDict buses_dict;
    for (const auto& item : Range{stops_end, end(data)}) {
        const auto& bus = get<Descriptions::Bus>(item);

        buses_dict[bus.name] = &bus;
        buses_[bus.name] = Bus{
                bus.stops.size(),
                ComputeUniqueItemsCount(AsRange(bus.stops)),
                ComputeRoadRouteLength(bus.stops, stops_dict),
                ComputeGeoRouteDistance(bus.stops, stops_dict),
                bus.stops,
                bus.is_roundtrip
        };

        for (const string& stop_name : bus.stops) {
            stops_.at(stop_name).bus_names.insert(bus.name);
        }
    }

    router_ = make_unique<TransportRouter>(stops_dict, buses_dict, routing_settings_json);
}

const TransportCatalog::Stop* TransportCatalog::GetStop(const string& name) const {
    return GetValuePointer(stops_, name);
}

const TransportCatalog::Bus* TransportCatalog::GetBus(const string& name) const {
    return GetValuePointer(buses_, name);
}

optional<TransportRouter::RouteInfo> TransportCatalog::FindRoute(const string& stop_from, const string& stop_to) const {
    return router_->FindRoute(stop_from, stop_to);
}

int TransportCatalog::ComputeRoadRouteLength(
        const vector<string>& stops,
        const Descriptions::StopsDict& stops_dict
) {
    int result = 0;
    for (size_t i = 1; i < stops.size(); ++i) {
        result += Descriptions::ComputeStopsDistance(*stops_dict.at(stops[i - 1]), *stops_dict.at(stops[i]));
    }
    return result;
}

double TransportCatalog::ComputeGeoRouteDistance(
        const vector<string>& stops,
        const Descriptions::StopsDict& stops_dict
) {
    double result = 0;
    for (size_t i = 1; i < stops.size(); ++i) {
        result += Sphere::Distance(
                stops_dict.at(stops[i - 1])->position, stops_dict.at(stops[i])->position
        );
    }
    return result;
}


std::string TransportCatalog::RenderMap() const {
    Svg::Document document;

    vector<double> lons, lats;

    for (const auto& [name, stop] : stops_) {
        lons.push_back(stop.position.longitude);
        lats.push_back(stop.position.latitude);
    }

    sort(lons.begin(), lons.end());
    sort(lats.begin(), lats.end());

    unordered_map<double, size_t> lon_idx;
    unordered_map<double, size_t> lat_idx;

    for (size_t i = 0; i < lons.size(); i++) {
        lon_idx[lons[i]] = i;
    }

    for (size_t i = 0; i < lats.size(); i++) {
        lat_idx[lats[i]] = i;
    }

    double x_step;
    double y_step;

    if (lon_idx.size() > 1) {
        x_step = (render_settings.width - 2 * render_settings.padding) / (lon_idx.size() - 1);
    } else {
        x_step = 0;
    }

    if (lat_idx.size() > 1) {
        y_step = (render_settings.height - 2 * render_settings.padding) / (lat_idx.size() - 1);
    } else {
        y_step = 0;
    }

    map<std::string, Bus> sorted_buses(buses_.begin(), buses_.end());
    std::map<std::string, Stop> sorted_stops(stops_.begin(), stops_.end());

    for (auto& layer_name : render_settings.layers) {
        if (layer_name == "bus_lines") {
            size_t idx = 0;

            for (const auto&[bus_name, bus] : sorted_buses) {
                Svg::Polyline line;

                line.SetStrokeColor(render_settings.color_palette[idx % render_settings.color_palette.size()]);
                idx++;

                line.SetStrokeWidth(render_settings.line_width);
                line.SetStrokeLineCap("round");
                line.SetStrokeLineJoin("round");
                //std::cerr << bus_name << std::endl;
                for (const auto &stop_name : bus.stop_names) {
                    //std::cerr << "\t" << stop_name << " ";
                    const Sphere::Point &point = stops_.at(stop_name).position;
                    // std::cerr << std::setprecision(6) << " " << point.latitude << " " << point.longitude << std::endl;

                    double x = lon_idx[point.longitude] * x_step + render_settings.padding;
                    double y = render_settings.height - render_settings.padding - lat_idx[point.latitude] * y_step;

                    line.AddPoint({x, y});
                }

                document.Add(line);
            }
        } else if (layer_name == "bus_labels") {
            size_t idx = 0;
            for (auto&[bus_name, bus] : sorted_buses) {
                const Stop &stop = stops_.at(bus.stop_names.front());

                const Sphere::Point &point = stop.position;

                double x = lon_idx[point.longitude] * x_step + render_settings.padding;
                double y = render_settings.height - render_settings.padding - lat_idx[point.latitude] * y_step;

                Svg::Text text, background;

                text.SetPoint({x, y});
                text.SetOffset(render_settings.bus_label_offset);
                text.SetFontSize(render_settings.bus_label_font_size);
                text.SetFontFamily("Verdana");
                text.SetFontWeight("bold");
                text.SetData(bus_name);

                background = text;

                background.SetFillColor(render_settings.underlayer_color);
                background.SetStrokeColor(render_settings.underlayer_color);
                background.SetStrokeWidth(render_settings.underlayer_width);
                background.SetStrokeLineCap("round");
                background.SetStrokeLineJoin("round");

                text.SetFillColor(render_settings.color_palette[idx % render_settings.color_palette.size()]);

                document.Add(background);
                document.Add(text);

                if (!bus.is_roundtrip && bus.stop_names[bus.stop_names.size() / 2] != bus.stop_names.front()) {
                    //std::cerr << bus_name << std::endl;
                    const Stop& stop = stops_.at(bus.stop_names[bus.stop_names.size() / 2]);

                    const Sphere::Point &point = stop.position;

                    double x = lon_idx[point.longitude] * x_step + render_settings.padding;
                    double y = render_settings.height - render_settings.padding - lat_idx[point.latitude] * y_step;

                    Svg::Text text, background;

                    text.SetPoint({x, y});
                    text.SetOffset(render_settings.bus_label_offset);
                    text.SetFontSize(render_settings.bus_label_font_size);
                    text.SetFontFamily("Verdana");
                    text.SetFontWeight("bold");
                    text.SetData(bus_name);

                    background = text;

                    background.SetFillColor(render_settings.underlayer_color);
                    background.SetStrokeColor(render_settings.underlayer_color);
                    background.SetStrokeWidth(render_settings.underlayer_width);
                    background.SetStrokeLineCap("round");
                    background.SetStrokeLineJoin("round");

                    text.SetFillColor(render_settings.color_palette[idx % render_settings.color_palette.size()]);

                    document.Add(background);
                    document.Add(text);
                }

                idx++;
            }
        } else if (layer_name == "stop_points") {
            for (auto&[stop_name, stop] : sorted_stops) {
                const Sphere::Point &point = stop.position;

                double x = lon_idx[point.longitude] * x_step + render_settings.padding;
                double y = render_settings.height - render_settings.padding - lat_idx[point.latitude] * y_step;

                Svg::Circle circle;
                circle.SetCenter({x, y});
                circle.SetRadius(render_settings.stop_radius);
                circle.SetFillColor("white");

                document.Add(circle);
            }
        } else if (layer_name == "stop_labels") {
            for (auto&[stop_name, stop] : sorted_stops) {
                const Sphere::Point &point = stop.position;

                double x = lon_idx[point.longitude] * x_step + render_settings.padding;
                double y = render_settings.height - render_settings.padding - lat_idx[point.latitude] * y_step;

                Svg::Circle circle;

                Svg::Text text, background;

                text.SetPoint({x, y});
                text.SetOffset(render_settings.stop_label_offset);
                text.SetFontSize(render_settings.stop_label_font_size);
                text.SetFontFamily("Verdana");
                text.SetData(stop_name);
                text.SetFillColor("black");

                background = text;

                background.SetFillColor(render_settings.underlayer_color);
                background.SetStrokeColor(render_settings.underlayer_color);
                background.SetStrokeWidth(render_settings.underlayer_width);
                background.SetStrokeLineJoin("round");
                background.SetStrokeLineCap("round");


                document.Add(background);
                document.Add(text);
            }
        }
    }

    return document.to_json_string();
}
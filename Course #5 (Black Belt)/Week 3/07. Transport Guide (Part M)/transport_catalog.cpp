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

    std::unordered_map<std::string, std::set<std::string>> neighbours;

    for (const auto& [bus_name, bus] : buses_) {
        const auto& stop_names = bus.stop_names;

        for (size_t i = 1; i < stop_names.size(); i++) {
            const std::string& from = stop_names[i - 1];
            const std::string& to = stop_names[i];

            neighbours[from].insert(to);
            neighbours[to].insert(from);
        }
    }

    std::unordered_map<std::string, double> lons_dict, lats_dict;

    std::unordered_set<std::string> bearing_stops;

    for (const auto& [bus_name, bus] : buses_) {
        bearing_stops.insert(bus.stop_names.front());
        if (!bus.is_roundtrip) {
            bearing_stops.insert(bus.stop_names[bus.stop_names.size() / 2]);
        }
    }

    for (const auto& [stop_name, stop] : stops_) {
        if (stop.bus_names.size() > 1 || stop.bus_names.empty()) {
            bearing_stops.insert(stop_name);
        } else if (stop.bus_names.size() == 1) {
            std::string bus_name = *stop.bus_names.begin();
            if (std::count(buses_.at(bus_name).stop_names.begin(), buses_.at(bus_name).stop_names.end(), stop_name) > 2) {
                bearing_stops.insert(stop_name);
            }
        }
    }

    for (const auto& [name, stop] : stops_) {
        if (bearing_stops.find(name) != bearing_stops.end()) {
            //std::cerr << name << std::endl;
            lons_dict[name] = stop.position.longitude;
            lats_dict[name] = stop.position.latitude;
        }
    }

    for (const auto& [bus_name, bus] : buses_) {
        const std::vector<std::string>& stops = bus.stop_names;

        int i = 0;
        for (int j = 1; j < stops.size(); j++) {
            if (bearing_stops.find(stops[j]) != bearing_stops.end()) {
                const string& from = stops[i];
                const string& to = stops[j];

                std::cerr << bus_name << " " << i << " " << j << " " << from << " " << to << std::endl;
                double lon_step = (stops_.at(to).position.longitude - stops_.at(from).position.longitude) / double(j - i);
                double lat_step = (stops_.at(to).position.latitude - stops_.at(from).position.latitude) / double(j - i);

                for (int k = i + 1; k < j; k++) {
                    std::cerr << bus_name << " " << stops[k] << std::endl;
                    lons_dict[stops[k]] = stops_.at(from).position.longitude + (lon_step * (k - i));
                    lats_dict[stops[k]] = stops_.at(from).position.latitude + (lat_step * (k - i));
                }
                std::cerr << endl;
                i = j;
            }
        }
    }

    std::vector<std::pair<double, std::string>> lons, lats;
    lons.reserve(lons_dict.size());
    lats.reserve(lats_dict.size());

    for (auto& [name, value] : lons_dict) {
        lons.emplace_back(value, name);
    }

    for (auto& [name, value] : lats_dict) {
        lats.emplace_back(value, name);
    }

    for (auto& [coord, name] : lons) {
        std::cerr << coord << " " << name << std::endl;
    }
    std::cerr << std::endl;
    for (auto& [coord, name] : lats) {
        std::cerr << coord << " " << name << std::endl;
    }

    sort(lons.begin(), lons.end());
    sort(lats.begin(), lats.end());

    unordered_map<double, size_t> lon_idx;
    unordered_map<double, size_t> lat_idx;

    lon_idx[lons[0].first] = 0;
    size_t max_lon_idx = 0;
    for (size_t i = 1; i < lons.size(); i++) {
        size_t idx = 0;

        for (size_t j = 0; j < i; j++) {
            std::string& from = lons[i].second;
            std::string& to = lons[j].second;

            if (neighbours[from].find(to) != neighbours[from].end()) {
                idx = max(idx, lon_idx[lons[j].first] + 1);
            }
        }

        lon_idx[lons[i].first] = idx;
        max_lon_idx = max(idx, max_lon_idx);
    }

    lat_idx[lats[0].first] = 0;
    size_t max_lat_idx = 0;
    for (int i = 1; i < lats.size(); i++) {
        size_t idx = 0;

        for (size_t j = 0; j < i; j++) {
            std::string& from = lats[i].second;
            std::string& to = lats[j].second;

            if (neighbours[from].find(to) != neighbours[from].end()) {
                idx = max(idx, lat_idx[lats[j].first] + 1);
            }
        }

        lat_idx[lats[i].first] = idx;
        max_lat_idx = max(idx, max_lat_idx);
    }

    double x_step;
    double y_step;

    if (max_lon_idx != 0) {
        x_step = (render_settings.width - 2 * render_settings.padding) / max_lon_idx;
    } else {
        x_step = 0;
    }

    if (max_lat_idx != 0) {
        y_step = (render_settings.height - 2 * render_settings.padding) / max_lat_idx;
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
                    double x = lon_idx[lons_dict[stop_name]] * x_step + render_settings.padding;
                    double y = render_settings.height - render_settings.padding - lat_idx[lats_dict[stop_name]] * y_step;

                    line.AddPoint({x, y});
                }

                document.Add(line);
            }
        } else if (layer_name == "bus_labels") {
            size_t idx = 0;
            for (auto&[bus_name, bus] : sorted_buses) {
                const Stop &stop = stops_.at(bus.stop_names.front());

                string& stop_name = bus.stop_names.front();
                double x = lon_idx[lons_dict[stop_name]] * x_step + render_settings.padding;
                double y = render_settings.height - render_settings.padding - lat_idx[lats_dict[stop_name]] * y_step;

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

                    const string& stop_name = bus.stop_names[bus.stop_names.size() / 2];
                    double x = lon_idx[lons_dict[stop_name]] * x_step + render_settings.padding;
                    double y = render_settings.height - render_settings.padding - lat_idx[lats_dict[stop_name]] * y_step;

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
                double x = lon_idx[lons_dict[stop_name]] * x_step + render_settings.padding;
                double y = render_settings.height - render_settings.padding - lat_idx[lats_dict[stop_name]] * y_step;

                Svg::Circle circle;
                circle.SetCenter({x, y});
                circle.SetRadius(render_settings.stop_radius);
                circle.SetFillColor("white");

                document.Add(circle);
            }
        } else if (layer_name == "stop_labels") {
            for (auto&[stop_name, stop] : sorted_stops) {
                double x = lon_idx[lons_dict[stop_name]] * x_step + render_settings.padding;
                double y = render_settings.height - render_settings.padding - lat_idx[lats_dict[stop_name]] * y_step;

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
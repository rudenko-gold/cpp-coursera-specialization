#pragma once

#include "descriptions.h"
#include "json.h"
#include "transport_router.h"
#include "utils.h"
#include "svg.h"
#include "sphere.h"

#include "transport_catalog.pb.h"

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <numeric>
#include <limits>

namespace Responses {
    struct Stop {
        std::set<std::string> bus_names;
        Sphere::Point position;
    };

    struct Bus {
        size_t stop_count = 0;
        size_t unique_stop_count = 0;
        int road_route_length = 0;
        double geo_route_length = 0.0;
        std::vector<std::string> stop_names;
        bool is_roundtrip;
    };

    struct RenderSettings {
        RenderSettings() = default;

        RenderSettings(const Json::Dict& render_settings_json) {
            width = render_settings_json.at("width").AsDouble();
            height = render_settings_json.at("height").AsDouble();
            padding = render_settings_json.at("padding").AsDouble();
            stop_radius = render_settings_json.at("stop_radius").AsDouble();
            line_width = render_settings_json.at("line_width").AsDouble();
            stop_label_font_size = render_settings_json.at("stop_label_font_size").AsInt();
            stop_label_offset.x = render_settings_json.at("stop_label_offset").AsArray()[0].AsDouble();
            stop_label_offset.y = render_settings_json.at("stop_label_offset").AsArray()[1].AsDouble();
            bus_label_font_size = render_settings_json.at("bus_label_font_size").AsInt();
            bus_label_offset.x = render_settings_json.at("bus_label_offset").AsArray()[0].AsDouble();
            bus_label_offset.y = render_settings_json.at("bus_label_offset").AsArray()[1].AsDouble();
            outer_margin = render_settings_json.at("outer_margin").AsDouble();

            const Json::Node& underlayer_color_node = render_settings_json.at("underlayer_color");

            if (underlayer_color_node.IsString()) {
                underlayer_color = underlayer_color_node.AsString();
            } else {
                const auto& underlayer_color_array = underlayer_color_node.AsArray();

                if (underlayer_color_array.size() == 3) {
                    Svg::Rgb rgb;

                    rgb.red = underlayer_color_array[0].AsInt();
                    rgb.green = underlayer_color_array[1].AsInt();
                    rgb.blue = underlayer_color_array[2].AsInt();

                    underlayer_color = rgb;
                } else {
                    Svg::Rgba rgba;

                    rgba.red = underlayer_color_array[0].AsInt();
                    rgba.green = underlayer_color_array[1].AsInt();
                    rgba.blue = underlayer_color_array[2].AsInt();
                    rgba.alpha = underlayer_color_array[3].AsDouble();

                    underlayer_color = rgba;
                }
            }

            underlayer_width = render_settings_json.at("underlayer_width").AsDouble();

            auto& color_palette_array = render_settings_json.at("color_palette").AsArray();

            for (auto& color_node : color_palette_array) {
                if (color_node.IsString()) {
                    color_palette.emplace_back(color_node.AsString());
                } else {
                    auto& color_node_array = color_node.AsArray();

                    if (color_node_array.size() == 3) {
                        Svg::Rgb rgb;

                        rgb.red = color_node_array[0].AsInt();
                        rgb.green = color_node_array[1].AsInt();
                        rgb.blue = color_node_array[2].AsInt();

                        color_palette.emplace_back(rgb);
                    } else {
                        Svg::Rgba rgba;

                        rgba.red = color_node_array[0].AsInt();
                        rgba.green = color_node_array[1].AsInt();
                        rgba.blue = color_node_array[2].AsInt();
                        rgba.alpha = color_node_array[3].AsDouble();

                        color_palette.emplace_back(rgba);
                    }
                }
            }

            const std::vector<Json::Node>& layers_array = render_settings_json.at("layers").AsArray();

            for (const auto& node : layers_array) {
                layers.push_back(node.AsString());
            }
        }

        double width;
        double height;
        double padding;
        double outer_margin;
        double stop_radius;
        double line_width;
        int stop_label_font_size;
        Svg::Point stop_label_offset;
        Svg::Color underlayer_color;
        double underlayer_width;
        std::vector<Svg::Color> color_palette;
        int bus_label_font_size;
        Svg::Point bus_label_offset;
        std::vector<std::string> layers;
    };
}

class TransportCatalog {
private:
    using Bus = Responses::Bus;
    using Stop = Responses::Stop;

public:
    TransportCatalog(std::vector<Descriptions::InputQuery> data, const Json::Dict& routing_settings_json, const Json::Dict& render_settings_json);
    TransportCatalog(const Serialized::TransportCatalog& serialized_transport_catalog);
    TransportCatalog(std::vector<Descriptions::InputQuery> data);

    const Stop* GetStop(const std::string& name) const;
    const Bus* GetBus(const std::string& name) const;

    std::optional<TransportRouter::RouteInfo> FindRoute(const std::string& stop_from, const std::string& stop_to) const;

    std::string RenderMap(const Svg::Document& document) const;
    std::string RenderRoute(const TransportRouter::RouteInfo& route) const;
    Svg::Document RenderMap() const;

    Serialized::TransportCatalog Serialize() const;
private:
    static int ComputeRoadRouteLength(
            const std::vector<std::string>& stops,
            const Descriptions::StopsDict& stops_dict
    );

    static double ComputeGeoRouteDistance(
            const std::vector<std::string>& stops,
            const Descriptions::StopsDict& stops_dict
    );

    Responses::RenderSettings render_settings;
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;
    std::unique_ptr<TransportRouter> router_;
};
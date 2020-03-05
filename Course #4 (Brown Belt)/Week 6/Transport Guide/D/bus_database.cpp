#include "bus_database.h"

bool operator== (const Route& lhs, const Route& rhs) {
    return lhs.title == rhs.title && lhs.stops == rhs.stops;
}

std::ostream& operator<< (std::ostream& os, const Route& route) {
    os << route.title;
    return os;
}

Route readRouteInfo(std::istream& is) {
    Route route;
    is.ignore(1);
    getline(is, route.title, ':');
    is.ignore(1);
    std::string input_line;
    getline(is, input_line);
    if (input_line.find('-') != std::string::npos) {
        route.stops = split(input_line, " - ");
        for (int i = route.stops.size() - 2; i >= 0; --i) {
            route.stops.push_back(route.stops[i]);
        }
    } else {
        route.stops = split(input_line, " > ");
    }

    return route;
}

Route readRouteInfo(const Json::Node& node) {
    Route route;

    auto const& node_dict = node.AsMap();

    route.title = node_dict.at("name").AsString();

    for (const auto& stop_node : node_dict.at("stops").AsArray()) {
        route.stops.push_back(stop_node.AsString());
    }

    if (!node_dict.at("is_roundtrip").AsBool()) {
        for (int i = route.stops.size() - 2; i >= 0; --i) {
            route.stops.push_back(route.stops[i]);
        }
    }

    return route;
}

void RouteDatabase::readDatabase(std::istream& is) {

}

void RouteDatabase::addRoute(Route route) {
    route_.insert({ route.title, route });
}

const Route& RouteDatabase::getRoute(const std::string& title) {
    return route_.at(title);
}

bool operator== (const RouteDatabase& lhs, const RouteDatabase& rhs) {
    return lhs.route_ == rhs.route_;
}
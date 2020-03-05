#include "stop_database.h"

bool operator==(const Stop& lhs, const Stop& rhs) {
    return lhs.title == rhs.title && lhs.coords == rhs.coords;
}

std::ostream& operator<<(std::ostream& os, const Stop& stop) {
    os << stop.title << "[" << stop.coords.getLatitude() << ", " << stop.coords.getLongitude() << "]";
    return os;
}

Stop readStopInfo(std::istream& is) {
    Stop stop;
    is.ignore(1);
    getline(is, stop.title, ':');
    is >> stop.coords;
    std::string other_info;
    getline(is, other_info);

    //std::cerr << "[" << other_info << "]" << std::endl;

    if (!other_info.empty()) {
        std::stringstream ss(other_info);
        ss.ignore(1);
        while (ss) {
            int distance;
            ss >> distance;
            ss.ignore(1);
            std::string trash;
            ss >> trash;
            std::string stop_title;
            ss.ignore(1);
            getline(ss, stop_title, ',');

            if(!stop_title.empty()) {
                //std::cerr << distance << " -> " << "[" << stop_title << "]\n";
                stop.distance_to_stop[stop_title] = distance;
            }
        }
    }
    return stop;
}

Stop readStopInfo(const Json::Node& node) {
    Stop stop;

    auto const& node_dict = node.AsMap();

    stop.title = node_dict.at("name").AsString();
    double longitude, latitude;

    try {
        longitude = node_dict.at("longitude").AsDouble();
    } catch(...) {
        longitude = node_dict.at("longitude").AsInt();
    }

    try {
        latitude = node_dict.at("latitude").AsDouble();
    } catch(...) {
        latitude = node_dict.at("latitude").AsInt();
    }

    stop.coords = geo::Point(latitude, longitude);

    auto const& to_other_stops = node_dict.at("road_distances").AsMap();

    for (const auto& [stop_title, distance_node] : to_other_stops) {
        //std::cerr << stop.title << " -> " << stop_title << ": " << distance_node.AsInt() << "\n";
        stop.distance_to_stop[stop_title] = distance_node.AsInt();
    }

    return stop;
}

void StopDatabase::readDatabase(std::istream& is) {

}

void StopDatabase::addStop(Stop stop) {
    stop_.insert({stop.title, stop });

    if (route_.find(stop.title) == route_.end()) {
        route_[stop.title] = {};
    }
}

void StopDatabase::addRoute(std::string stop_title, std::string route_title) {
    route_[stop_title].insert(route_title);
}

const Stop& StopDatabase::getStop(const std::string& title) const {
    return stop_.at(title);
}

std::set<std::string> StopDatabase::getRoutes(const std::string& title) const {
    return route_.at(title);
}

bool operator==(const StopDatabase& lhs, const StopDatabase& rhs) {
    return lhs.stop_ == rhs.stop_;
}
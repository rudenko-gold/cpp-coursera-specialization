#include "transport_manager.h"

#include <set>

void TransportManager::readData(const Json::Document& input) {
    const auto& queries_array = input.GetRoot().AsMap().at("base_requests").AsArray();

    for (auto const& query_node : queries_array) {
        auto const& query = query_node.AsMap();
        auto const& type_node = query.at("type");

        if (type_node.AsString() == "Stop") {
            stop_db.addStop(readStopInfo(query));
        } else if (type_node.AsString() == "Bus") {

            Route route = readRouteInfo(query);
            //std::cerr << "[ " << route.title << "\n";
            for (auto& stop : route.stops) {
                //std::cerr  << stop  << "\n";
                stop_db.addRoute(stop, route.title);
            }
            //std::cerr << "]\n";

            route_db.addRoute(std::move(route));
        }

    }

    /*
    int query_number = 0;
    is >> query_number;
    while (query_number--) {
        std::string type;
        is >> type;
        if (type == "Stop") {
            stop_db.addStop(readStopInfo(is));
        } else {
            Route route = readRouteInfo(is);

            for (auto& stop : route.stops) {
                stop_db.addRoute(stop, route.title);
            }

            route_db.addRoute(std::move(route));

        }
    }
    */
}

void TransportManager::performQueries(std::istream& is) {
    const Json::Document input = Json::Load(is);
    readData(input);

    const auto &queries_array = input.GetRoot().AsMap().at("stat_requests").AsArray();

    std::cout << "[\n";

    for (size_t i = 0; i < queries_array.size(); ++i) {
        const auto &query_dict = queries_array[i].AsMap();
        int request_id = query_dict.at("id").AsInt();

        if (query_dict.at("type").AsString() == "Bus") {
            std::string title = query_dict.at("name").AsString();
            RouteResponse response = performRouteQuery(title);
            //std::cout << response << "\n";

            if (response.exist) {
                std::map<std::string, std::string> route_stats = {
                        {"\"route_length\"",      std::to_string(response.route_real_length)},
                        {"\"request_id\"",        std::to_string(request_id)},
                        {"\"curvature\"",         std::to_string(response.curvature)},
                        {"\"stop_count\"",        std::to_string(response.stops_number)},
                        {"\"unique_stop_count\"", std::to_string(response.unique_stops_number)}
                };
                std::cout << route_stats;
            } else {
                std::map<std::string, std::string> route_stats = {
                        {"\"request_id\"", std::to_string(request_id)},
                        { "\"error_message\"", "\"not found\"" }
                };
                std::cout << route_stats;
            }
        } else if (query_dict.at("type").AsString() == "Stop") {
            std::string title = query_dict.at("name").AsString();
            StopResponse response = performStopQuery(title);
            //std::cout << response << std::endl;

            if (response.exist) {
                std::vector<std::string> raw_routes(response.routes.begin(), response.routes.end());
                std::string routes_str = "[";
                for (const auto& raw_route : raw_routes) {
                    routes_str += "\"" + raw_route + "\",";
                }
                if (!raw_routes.empty()) {
                    routes_str.pop_back();
                }
                routes_str += "]";

                std::map<std::string, std::string> stop_stats = {
                        { "\"buses\"", routes_str },
                        {"\"request_id\"", std::to_string(request_id)}
                };
                std::cout << stop_stats;
            } else {
                std::map<std::string, std::string> stop_stats = {
                        {"\"request_id\"", std::to_string(request_id)},
                        { "\"error_message\"", "\"not found\"" }
                };
                std::cout << stop_stats;
            }
        }

        if (i != queries_array.size() - 1) {
            std::cout << ",";
        }
        std::cout << "\n";
    }
    std::cout << "]\n";
        /*
        int query_number = 0;
        is >> query_number;
        while (query_number--) {
            std::string query_type;
            is >> query_type;

            if  (query_type == "Bus") {
                std::string route_title = "";
                is.ignore(1);
                getline(is, route_title);
                std::cout << performRouteQuery(route_title) << std::endl;
            } else {
                std::string stop_title = "";
                is.ignore(1);
                getline(is, stop_title);
                std::cout << performStopQuery(stop_title) << std::endl;
            }
        }
        */
}

double TransportManager::computeDirectDistance(const Route& route) {
    double distance = 0.0;

    for (size_t i = 1; i < route.stops.size(); ++i) {
        const std::string& stop_title = route.stops[i];
        const std::string& prev_stop_title = route.stops[i - 1];

        const Stop& start = stop_db.getStop(prev_stop_title);
        const Stop& finish = stop_db.getStop(stop_title);
        distance += geo::Point::computeDistance(start.coords, finish.coords);
    }

    return distance;
}

int TransportManager::computeRealDistance(const Route& route) {
    int distance = 0;

    for (size_t i = 1; i < route.stops.size(); ++i) {
        const std::string& stop_title = route.stops[i];
        const std::string& prev_stop_title = route.stops[i - 1];

        const Stop& start = stop_db.getStop(prev_stop_title);
        const Stop& finish = stop_db.getStop(stop_title);

        if (start.distance_to_stop.find(finish.title) != start.distance_to_stop.end()) {
            distance += start.distance_to_stop.at(finish.title);
        } else {
            distance += finish.distance_to_stop.at(start.title);
        }
    }

    return distance;
}

RouteResponse TransportManager::performRouteQuery(const std::string& title) {
    RouteResponse response;
    response.title = title;
    try {
        const Route &route = route_db.getRoute(title);

        response.stops_number = route.stops.size();
        response.unique_stops_number = std::set<std::string>(route.stops.begin(), route.stops.end()).size();
        response.route_direct_length = computeDirectDistance(route);
        response.route_real_length = computeRealDistance(route);
        response.curvature = response.route_real_length / response.route_direct_length;

        return response;
    } catch(...) {
        response.exist = false;
        return response;
    }
}

StopResponse TransportManager::performStopQuery(const std::string &title) {
    StopResponse response;
    response.title = title;

    try {
        response.routes = stop_db.getRoutes(title);
        return response;
    } catch(...) {

        response.exist = false;
        return response;
    }
}

bool operator==(const TransportManager& lhs, const TransportManager& rhs) {
    return lhs.route_db == rhs.route_db && lhs.stop_db == rhs.stop_db;
}

std::ostream& operator<<(std::ostream& os, const TransportManager& mng) {
    return os;
}

std::ostream& operator<<(std::ostream& os, const RouteResponse& response) {
    os << "Bus " << response.title << ": ";

    if (response.exist) {
        os << response.stops_number << " stops on route, ";
        os << response.unique_stops_number << " unique stops, ";
        os << response.route_real_length << " route length, ";
        os << std::fixed << std::setprecision(6) << response.curvature << " curvature";
    } else {
        os << "not found";
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const StopResponse& response) {
    os << "Stop " << response.title << ": ";

    if (response.exist) {
        if (response.routes.empty()) {
            os << "no buses";
        } else {
            os << "buses ";
            for (auto& bus : response.routes) {
                os << bus << " ";
            }
        }
    } else {
        os << "not found";
    }
    return os;
}


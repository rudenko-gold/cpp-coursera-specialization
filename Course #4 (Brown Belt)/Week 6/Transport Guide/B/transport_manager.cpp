#include "transport_manager.h"

#include <set>

void TransportManager::readData(std::istream& is) {
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
}

void TransportManager::performQueries(std::istream& is) {
    readData(is);
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
}

RouteResponse TransportManager::performRouteQuery(const std::string& title) {
    RouteResponse response;
    response.title = title;
    try {
        const Route &route = route_db.getRoute(title);

        response.stops_number = route.stops.size();
        response.unique_stops_number = std::set<std::string>(route.stops.begin(), route.stops.end()).size();

        for (size_t i = 1; i < route.stops.size(); ++i) {
            const std::string& stop_title = route.stops[i];
            const std::string& prev_stop_title = route.stops[i - 1];

            const Stop& start = stop_db.getStop(prev_stop_title);
            const Stop& finish = stop_db.getStop(stop_title);
            response.route_length += geo::Point::computeDistance(start.coords, finish.coords);
        }

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
        os << std::fixed << std::setprecision(6) << response.route_length << " route length";
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


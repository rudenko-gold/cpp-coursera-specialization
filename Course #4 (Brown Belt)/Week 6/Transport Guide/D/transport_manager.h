#pragma once

#include "string_util.h"
#include "bus_database.h"
#include "stop_database.h"
#include "json.h"
#include "sstream"
#include <utility>
#include <iostream>
#include <iomanip>

struct RouteResponse {
    std::string title;
    int stops_number = 0;
    int unique_stops_number = 0;
    int route_real_length = 0;
    double route_direct_length = 0.0;
    double curvature;
    bool exist = true;
};

struct StopResponse {
    std::string title;
    std::set<std::string> routes;
    bool exist = true;
};

std::ostream& operator<<(std::ostream& os, const RouteResponse& response);
std::ostream& operator<<(std::ostream& os, const StopResponse& response);

class TransportManager {
public:
    TransportManager() {};

    TransportManager(RouteDatabase routes, StopDatabase stops) : route_db(std::move(routes)), stop_db(std::move(stops)) {}
    void performQueries(std::istream& is);
    void readData(const Json::Document& input);
private:
    double computeDirectDistance(const Route& route);
    int computeRealDistance(const Route& route);

    RouteDatabase route_db;
    StopDatabase stop_db;
    RouteResponse performRouteQuery(const std::string& title);
    StopResponse performStopQuery(const std::string& title);
    friend bool operator==(const TransportManager& lhs, const TransportManager& rhs);
    friend std::ostream& operator<<(std::ostream& os, const TransportManager& mng);
};
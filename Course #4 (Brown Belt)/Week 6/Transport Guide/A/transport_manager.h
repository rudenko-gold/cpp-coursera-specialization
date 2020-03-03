#pragma once

#include "string_util.h"
#include "bus_database.h"
#include "stop_database.h"
#include <utility>
#include <iostream>
#include <iomanip>

struct RouteResponse {
    std::string title;
    int stops_number = 0;
    int unique_stops_number = 0;
    double route_length = 0.0;
    bool exist = true;
};

std::ostream& operator<<(std::ostream& os, const RouteResponse& response);

class TransportManager {
public:
    TransportManager() {};

    TransportManager(RouteDatabase routes, StopDatabase stops) : route_db(std::move(routes)), stop_db(std::move(stops)) {}
    void performQueries(std::istream& is);
    void readData(std::istream& is);
private:
    RouteDatabase route_db;
    StopDatabase stop_db;
    RouteResponse performRouteQuery(const std::string& title);
    friend bool operator==(const TransportManager& lhs, const TransportManager& rhs);
    friend std::ostream& operator<<(std::ostream& os, const TransportManager& mng);
};
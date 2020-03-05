#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include "string_util.h"
#include "json.h"

class Route {
public:
    std::string title;
    std::vector<std::string> stops;
};

bool operator== (const Route& lhs, const Route& rhs);
std::ostream& operator<< (std::ostream& os, const Route& route);

Route readRouteInfo(std::istream& is);
Route readRouteInfo(const Json::Node& node);
class RouteDatabase {
public:
    void readDatabase(std::istream& is);
    void addRoute(Route route);
    const Route& getRoute(const std::string& title);
private:
    std::unordered_map<std::string, Route> route_;
    friend bool operator== (const RouteDatabase& lhs, const RouteDatabase& rhs);
};

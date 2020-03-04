#pragma once

#include "geo_util.h"
#include "string_util.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <set>

class Stop {
public:
    std::string title;
    geo::Point coords;
};

bool operator==(const Stop& lhs, const Stop& rhs);
std::ostream& operator<<(std::ostream& os, const Stop& stop);

Stop readStopInfo(std::istream& is);

class StopDatabase {
public:
    void readDatabase(std::istream& is);
    void addStop(Stop stop);
    void addRoute(std::string stop_title, std::string route_title);
    [[nodiscard]] std::set<std::string> getRoutes(const std::string& title) const;
    [[nodiscard]] const Stop& getStop(const std::string& title) const;
private:
    std::unordered_map<std::string, Stop> stop_;
    std::unordered_map<std::string, std::set<std::string>> route_;
    friend bool operator==(const StopDatabase& lhs, const StopDatabase& rhs);
};

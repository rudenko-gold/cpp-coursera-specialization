#pragma once

#include "geo_util.h"
#include "string_util.h"
#include <string>
#include <unordered_map>
#include <iostream>

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
    [[nodiscard]] const Stop& getStop(const std::string& title) const;
private:
    std::unordered_map<std::string, Stop> stop_;
    friend bool operator==(const StopDatabase& lhs, const StopDatabase& rhs);
};

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
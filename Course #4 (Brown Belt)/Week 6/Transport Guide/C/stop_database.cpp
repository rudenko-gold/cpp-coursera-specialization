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
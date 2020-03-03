#include "geo_util.h"

namespace geo {
    double Point::computeDistance(const Point& lhs, const Point& rhs) {
        constexpr double pi_per_deg = PI / 180;

        const double lat1 = lhs.latitude_ * pi_per_deg;
        const double lat2 = rhs.latitude_ * pi_per_deg;

        const double long1 = lhs.longitude_ * pi_per_deg;
        const double long2 = rhs.longitude_ * pi_per_deg;

        return std::acos(std::sin(lat1) * std::sin(lat2) +
                         std::cos(lat1) * std::cos(lat2) *
                         std::cos(std::abs(long1 - long2))) * EARTH_RADIUS;
    }

    double Point::getLatitude() const {
        return latitude_;
    }

    double Point::getLongitude() const {
        return longitude_;
    }

    bool operator== (const Point& lhs, const Point& rhs) {
        return fabs(lhs.longitude_ - rhs.longitude_) < EPS && fabs(lhs.latitude_ - rhs.latitude_) < EPS;
    }

    std::istream& operator>> (std::istream& is, Point& point) {
        is >> point.latitude_;
        is.ignore(1);
        is >> point.longitude_;
        return is;
    }

    std::ostream& operator<<(std::ostream& os, Point& point) {
        os << point.latitude_ << " " << point.longitude_;
        return os;
    }
}

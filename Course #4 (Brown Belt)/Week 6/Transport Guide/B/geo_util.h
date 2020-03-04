#pragma once

#include <cmath>
#include <istream>
#include <istream>
#include <cmath>

namespace geo {
    constexpr double EARTH_RADIUS = 6371000.0;
    constexpr double PI = 3.1415926535;
    constexpr double EPS = 0.000001;

    class Point {
    public:
        Point() {
            latitude_ = longitude_ = 0;
        }

        Point(double latitude, double longitude) {
            latitude_ = latitude;
            longitude_ =  longitude;
        }

        double getLatitude() const;
        double getLongitude() const;
        static double computeDistance(const Point& lhs, const Point& rhs);
    private:
        double latitude_;
        double longitude_;
        friend bool operator== (const Point& lhs, const Point& rhs);
        friend std::istream& operator>>(std::istream& is, Point& point);
        friend std::ostream& operator<<(std::ostream& os, Point& point);
    };


}
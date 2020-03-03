#pragma once

#include "bus_database.h"
#include "test_runner.h"
#include <sstream>

void test_route_input() {
    {
        std::stringstream ss("256: Biryulyovo Zapadnoye > Biryusinka > Universam >"
                             " Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya >"
                             " Biryulyovo Zapadnoye");

        Route expected;
        expected.title = "256";
        expected.stops = { "Biryulyovo Zapadnoye",
                           "Biryusinka",
                           "Universam",
                           "Biryulyovo Tovarnaya",
                           "Biryulyovo Passazhirskaya",
                           "Biryulyovo Zapadnoye" };

        Route result = readRouteInfo(ss);


        std::cout << "[" << result.title << "]" << std::endl;
        std::cout << "[" << expected.title << "]" << std::endl;

        for (auto& stop : result.stops) {
            std::cout << "[" << stop << "]" << std::endl;
        }

        std::cout << "------\n";

        for (auto& stop : expected.stops) {
            std::cout << "[" << stop << "]" << std::endl;
        }


        ASSERT_EQUAL(expected, result)
    }
    {
        std::stringstream  ss("750: Tolstopaltsevo - Marushkino - Rasskazovka");

        Route expected;
        expected.title = "750";
        expected.stops = { "Tolstopaltsevo",
                           "Marushkino",
                           "Rasskazovka",
                           "Marushkino",
                           "Tolstopaltsevo" };

        Route result = readRouteInfo(ss);
        ASSERT_EQUAL(expected, result)
    }
}
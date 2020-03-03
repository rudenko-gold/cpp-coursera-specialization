#pragma once

#include "stop_database.h"
#include "test_runner.h"
#include <sstream>

void test_stop_input() {
    {
        std::stringstream ss(" Biryulyovo Zapadnoye: 55.574371, 37.6517");

        Stop expected;
        expected.title = "Biryulyovo Zapadnoye";
        std::stringstream("55.574371, 37.6517") >> expected.coords;
        Stop result = readStopInfo(ss);
        ASSERT_EQUAL(expected, result);
    }
}
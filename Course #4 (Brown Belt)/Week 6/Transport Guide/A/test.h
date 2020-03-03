#pragma once
#include "test_runner.h"
#include "string_util_test.h"
#include "test_route_db.h"
#include "test_stop_db.h"
#include "transport_manager_test.h"

void test_all() {
    TestRunner tr;

    RUN_TEST(tr, test_split_char_sep);
    RUN_TEST(tr, test_split_string_sep);
    RUN_TEST(tr, test_route_input);
    RUN_TEST(tr, test_stop_input);
}

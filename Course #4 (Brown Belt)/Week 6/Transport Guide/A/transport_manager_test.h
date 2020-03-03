#pragma once

#include "test_runner.h"
#include "transport_manager.h"
#include <sstream>

void test_reading_data() {
    {
        RouteDatabase r_db;
        StopDatabase s_db;
        r_db.addRoute({ "256", { "Biryulyovo Zapadnoye",
                                 "Biryusinka",
                                 "Universam",
                                 "Biryulyovo",
                                 "Tovarnaya",
                                 "Biryulyovo",
                                 "Passazhirskaya",
                                 "Biryulyovo Zapadnoye" } });
        r_db.addRoute( {"750", { "Tolstopaltsevo",
                                 "Marushkino",
                                 "Rasskazovka",
                                 "Marushkino",
                                 "Tolstopaltsevo" } } );

        s_db.addStop({"Tolstopaltsevo", { 55.611087, 37.20829 } });
        s_db.addStop( {"Marushkino", { 55.595884, 37.209755 } });
        s_db.addStop({ "Rasskazovka", { 55.632761, 37.333324} });
        s_db.addStop({ "Biryulyovo Zapadnoye", { 55.574371, 37.6517 } });
        s_db.addStop( { "Biryusinka", { 55.581065, 37.64839 } });
        s_db.addStop( { "Universam", { 55.587655, 37.645687 } });
        s_db.addStop({ "Biryulyovo Tovarnaya", { 55.592028, 37.653656 } });
        s_db.addStop( {"Biryulyovo Passazhirskaya", { 55.580999, 37.659164 } });

        TransportManager expected(r_db, s_db);

        TransportManager result;
        result.readData(std::cin);
        ASSERT_EQUAL(expected, result);
    }
}

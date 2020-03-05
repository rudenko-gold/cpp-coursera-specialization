#include "transport_manager.h"

using namespace Json;

int main() {
    //test_all();
    //freopen("/Users/rudenko_gold/CLionProjects/navigaror_project/input.txt", "r", stdin);

    TransportManager manager;
    manager.performQueries(std::cin);

    return 0;
}
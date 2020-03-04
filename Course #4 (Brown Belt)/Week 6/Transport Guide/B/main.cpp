#include "transport_manager.h"

int main() {
    //test_all();

    TransportManager manager;
    manager.performQueries(std::cin);

    return 0;
}
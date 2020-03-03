#include "transport_manager.h"
#include "test.h"

int main() {
    test_all();

    TransportManager manager;
    manager.performQueries(std::cin);

    return 0;
}

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <deque>
#include <list>
#include <stack>
#include <random>

using namespace std;

std::int64_t MAX = numeric_limits<std::int64_t>::max();
std::int64_t MIN = numeric_limits<std::int64_t>::min();

std::int64_t MAXu = numeric_limits<std::uint64_t>::max();
std::int64_t MINu = numeric_limits<std::uint64_t>::min();

int64_t sum(int64_t lhs, int64_t rhs) {
    if (rhs >= 0) {
        std::uint64_t can_add;
        if (lhs >= 0) {
            can_add = MAX - lhs;
        } else {
            can_add = uint64_t(MAX) + uint64_t(abs(lhs));
        }
        if (can_add >= rhs) {
            return lhs + rhs;
        } else {
            throw invalid_argument("Overflow!");
        }
    } else {
        std::uint64_t can_remove;

        if (lhs >= 0) {
            can_remove = uint64_t(MAX) + 1ull + uint64_t(lhs);
        } else {
            can_remove = lhs - MIN;
        }
        uint64_t abs_rhs = uint64_t(abs(rhs + 1ll)) + 1ull;
        if (can_remove >= abs_rhs) {
            return lhs + rhs;
        } else {
            throw invalid_argument("Overflow!");
        }
    }
}

int main() {
    std::int64_t lhs, rhs;

    cin >> lhs >> rhs;

    try {
        std::cout << sum(lhs, rhs) << std::endl;
    } catch(...) {
        std::cout << "Overflow!" << std::endl;
    }
    
    return 0;
}

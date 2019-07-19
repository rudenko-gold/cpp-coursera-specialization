#include "test_runner.h"
#include <algorithm>
#include <memory>
#include <vector>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    if (range_end - range_begin < 2) {
        return;
    } else {
        size_t len = range_end - range_begin;

        MergeSort(range_begin, range_begin + len / 3);
        MergeSort(range_begin + len / 3, range_begin + (len * 2) / 3);
        MergeSort(range_begin + (len * 2) / 3, range_end);

        vector<typename RandomIt::value_type> temp1, temp2;

        merge(make_move_iterator(range_begin),
                make_move_iterator(range_begin + len / 3),
                make_move_iterator(range_begin + len / 3),
                make_move_iterator(range_begin + (len * 2) / 3),
                back_inserter(temp1));

        merge(make_move_iterator(temp1.begin()),
                make_move_iterator(temp1.end()),
                make_move_iterator(range_begin + (len * 2)/ 3),
                make_move_iterator(range_end),
                back_inserter(temp2));

        move(temp2.begin(), temp2.end(), range_begin);
    }
}

void TestIntVector() {
    vector<int> numbers = {6, 1, 3, 9, 1, 9, 8, 12, 1};
    MergeSort(begin(numbers), end(numbers));
    ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestIntVector);
    return 0;
}

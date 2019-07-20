#include "test_runner.h"
#include <vector>
#include <future>
#include <cmath>

using namespace std;

int64_t CalculateSubMatrixSum(const vector<vector<int>>& matrix, size_t first_row, size_t row_number) {
    int64_t result = 0;

    for (size_t row = 0; row < row_number && first_row + row < matrix.size(); ++row) {
        for (size_t col = 0; col < matrix.size(); ++col) {
            result += matrix[row + first_row][col];
        }
    }

    return result;
}

int64_t CalculateMatrixSum(const vector<vector<int>>& matrix) {
    vector<future<int64_t>> futures;
    size_t page_size = 0;

    if (matrix.size() % 4 == 0) {
        page_size = matrix.size() / 4;
    } else {
        page_size = (matrix.size() / 4) + 1;
    }

    for (size_t first_row = 0; first_row < matrix.size(); first_row += page_size) {
        futures.push_back(async([&matrix, page_size, first_row] () {
            return CalculateSubMatrixSum(matrix, first_row, page_size);
        }));
    }

    int64_t result = 0;
    for (auto& f : futures) {
        result += f.get();
    }
    return result;
}

void TestCalculateMatrixSum() {
    const vector<vector<int>> matrix = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12},
            {13, 14, 15, 16}
    };
    ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestCalculateMatrixSum);
}

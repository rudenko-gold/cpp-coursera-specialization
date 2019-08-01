#include "animals.h"
#include "test_runner.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

using namespace std;

using Zoo = vector<unique_ptr<Animal>>;

Zoo CreateZoo(istream& in) {
    Zoo zoo;
    string word;
    while (in >> word) {
        if (word == "Tiger") {
            auto t = make_unique<Tiger>();
            zoo.push_back(move(t));
        } else if (word == "Wolf") {
            auto w = make_unique<Wolf>();
            zoo.push_back(move(w));
        } else if (word == "Fox") {
            auto f = make_unique<Fox>();
            zoo.push_back(move(f));
        } else {
            throw runtime_error("Unknown animal!");
        }
    }
    return zoo;
}

void Process(const Zoo& zoo, ostream& out) {
    for (const auto& animal : zoo) {
        out << animal->Voice() << "\n";
    }
}

void TestZoo() {
    istringstream input("Tiger Wolf Fox Tiger");
    ostringstream output;
    Process(CreateZoo(input), output);

    const string expected =
            "Rrrr\n"
            "Wooo\n"
            "Tyaf\n"
            "Rrrr\n";

    ASSERT_EQUAL(output.str(), expected);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestZoo);
}

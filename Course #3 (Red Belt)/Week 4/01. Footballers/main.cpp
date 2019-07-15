#include <iostream>
#include <list>
#include <unordered_map>

int main () {
    size_t footballersAmount;
    std::cin >> footballersAmount;

    std::list<int> footballers;
    std::unordered_map<int, std::list<int>::iterator> footballersIterators;
    for (size_t i = 0; i < footballersAmount; ++i) {
        size_t footballerNumber, nextFootballerNumber;
        std::cin >> footballerNumber >> nextFootballerNumber;

        if (footballersIterators.find(nextFootballerNumber) == footballersIterators.end()) {

            footballers.push_back(footballerNumber);
            footballersIterators[footballerNumber] = --footballers.end();

        } else {

            footballersIterators[footballerNumber] =
                    footballers.insert(footballersIterators[nextFootballerNumber], footballerNumber);

        }
    }

    for (auto footballerNumber : footballers) {
        std::cout << footballerNumber << " ";
    }

    return 0;
}

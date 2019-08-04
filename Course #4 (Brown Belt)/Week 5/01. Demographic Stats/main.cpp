#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

enum class Gender {
    FEMALE,
    MALE
};

struct Person {
    int age;
    Gender gender;
    bool is_employed;
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
    if (range_begin == range_end) {
        return 0;
    }
    vector<typename InputIt::value_type> range_copy(range_begin, range_end);
    auto middle = begin(range_copy) + range_copy.size() / 2;
    nth_element(
            begin(range_copy), middle, end(range_copy),
            [](const Person& lhs, const Person& rhs) {
                return lhs.age < rhs.age;
            }
    );
    return middle->age;
}

template <typename Pred>
int ComputeMedianAge(vector<Person>& persons, Pred pred) {
    auto partition_end = partition(persons.begin(), persons.end(), pred);
    return ComputeMedianAge(persons.begin(), partition_end);
}

void PrintStats(vector<Person> persons) {
    cout << "Median age = " << ComputeMedianAge(persons.begin(), persons.end()) << endl;
    cout << "Medin age for females = " << ComputeMedianAge(persons, [](const Person& person) {
        return person.gender == Gender::FEMALE;
    }) << endl;
    cout << "Medin age for males = " << ComputeMedianAge(persons, [](const Person& person) {
        return person.gender == Gender::MALE;
    }) << endl;
    cout << "Medin age for employed females = " << ComputeMedianAge(persons, [](const Person& person) {
        return person.gender == Gender::FEMALE && person.is_employed;
    }) << endl;
    cout << "Medin age for unemployed females = " << ComputeMedianAge(persons, [](const Person& person) {
        return person.gender == Gender::FEMALE && !person.is_employed;
    }) << endl;
    cout << "Medin age for employed males = " << ComputeMedianAge(persons, [](const Person& person) {
        return person.gender == Gender::MALE && person.is_employed;
    }) << endl;
    cout << "Medin age for unemployed males = " << ComputeMedianAge(persons, [](const Person& person) {
        return person.gender == Gender::MALE && !person.is_employed;
    }) << endl;

    /*
        Median age = 40
        Median age for females = 40
        Median age for males = 55
        Median age for employed females = 40
        Median age for unemployed females = 80
        Median age for employed males = 55
        Median age for unemployed males = 78
    */
}

int main() {
    vector<Person> persons = {
            {31, Gender::MALE, false},
            {40, Gender::FEMALE, true},
            {24, Gender::MALE, true},
            {20, Gender::FEMALE, true},
            {80, Gender::FEMALE, false},
            {78, Gender::MALE, false},
            {10, Gender::FEMALE, false},
            {55, Gender::MALE, true},
    };
    PrintStats(persons);
    return 0;
}

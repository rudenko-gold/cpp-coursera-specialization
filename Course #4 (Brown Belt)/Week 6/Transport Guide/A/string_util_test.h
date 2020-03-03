#include "string_util.h"
#include "test_runner.h"

#include <string_view>
#include <string>
#include <vector>

void test_split_char_sep() {
    std::string input = "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n";
    std::vector<std::string> expected = {
            "Biryulyovo Zapadnoye ",
            " Biryusinka ",
            " Universam ",
            " Biryulyovo Tovarnaya ",
            " Biryulyovo Passazhirskaya ",
            " Biryulyovo Zapadnoye\n"
    };

    std::vector<std::string> answer = split(input, '>');
    ASSERT_EQUAL(expected, answer);
}

void test_split_string_sep() {
    std::string input = "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n";
    std::vector<std::string> expected = {
            "Biryulyovo Zapadnoye",
            "Biryusinka",
            "Universam",
            "Biryulyovo Tovarnaya",
            "Biryulyovo Passazhirskaya",
            "Biryulyovo Zapadnoye\n"
    };

    std::vector<std::string> answer = split(input, " > ");
    ASSERT_EQUAL(expected, answer);
}
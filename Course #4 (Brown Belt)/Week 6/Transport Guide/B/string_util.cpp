#include "string_util.h"

std::vector<std::string> split(const std::string& s, const char separator = ' ') {
    std::string_view str = s;
    std::vector<std::string> result;

    while (true) {
        size_t space = str.find(separator);
        result.emplace_back(std::string(str.substr(0, space)));

        if (space == std::string_view::npos) {
            break;
        } else {
            str.remove_prefix(space + 1);
        }
    }

    return result;
}

std::vector<std::string> split(const std::string& s, const std::string& separator = " ") {
    std::string_view str = s;
    std::vector<std::string> result;

    while (true) {
        size_t space = str.find(separator);
        result.emplace_back(std::string(str.substr(0, space)));

        if (space == std::string_view::npos) {
            break;
        } else {
            str.remove_prefix(space + separator.size());
        }
    }

    return result;
}


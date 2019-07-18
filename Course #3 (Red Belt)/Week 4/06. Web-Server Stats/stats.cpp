#include "stats.h"
#include <vector>
#include <string>
#include <algorithm>

Stats::Stats() {
    const vector<string_view> methodTitles = { "GET", "POST", "PUT", "DELETE", "UNKNOWN"};
    const vector<string_view> uriTitles = { "/", "/order", "/product", "/basket", "/help", "unknown" };

    for (string_view method : methodTitles) {
        methodStats[method] = 0;
    }

    for (string_view uri : uriTitles) {
        uriStats[uri] = 0;
    }
}

void Stats::AddMethod(string_view method) {
    const vector<string_view> methodTitles = { "GET", "POST", "PUT", "DELETE" };
    const string_view unknownMethod = "UNKNOWN";

    if (find(methodTitles.begin(), methodTitles.end(), method) == methodTitles.end()) {
        methodStats[unknownMethod]++;
    } else {
        methodStats[method]++;
    }
}

void Stats::AddUri(string_view uri) {
    const vector<string_view> uriTitles = { "/", "/order", "/product", "/basket", "/help" };
    const string_view unknownUri = "unknown";

    if (find(uriTitles.begin(), uriTitles.end(), uri) == uriTitles.end()) {
        uriStats[unknownUri]++;
    } else {
        uriStats[uri]++;
    }
}

const map<string_view, int>& Stats::GetMethodStats() const {
    return methodStats;
}

const map<string_view, int>& Stats::GetUriStats() const {
    return uriStats;
}

vector<string_view> parseWords(string_view line) {
    while (line.size() > 0 && line[0] == ' ') {
        line.remove_prefix(1);
    }

    vector<string_view> result;
    size_t pos = 0;
    const size_t posEnd = line.npos;

    while (true) {
        size_t space = line.find(' ', pos);
        if (space == posEnd) {
            result.push_back(line.substr(pos));
            break;
        } else {
            result.push_back(line.substr(pos, space - pos));
            pos = space + 1;
        }
    }

    return result;
}

HttpRequest ParseRequest(string_view line) {
    HttpRequest result;
    vector<string_view> splittedLine = parseWords(line);
    
    result.method = splittedLine[0];
    result.uri = splittedLine[1];
    result.protocol = splittedLine[2];

    return result;
}

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <map>

using namespace std;

vector<string> ReadDomains() {
    size_t count;
    cin >> count;

    vector<string> domains;
    domains.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        string domain;
        cin >> domain;
        domains.push_back(domain);
    }
    return domains;
}

unordered_set<string_view> getSubDomains(const vector<string>& domains) {
    unordered_set<string_view> subdomains;
    for (const string& domain : domains) {
       subdomains.insert(domain);
    }
    return subdomains;
}

bool isBanned(string_view domain, const unordered_set<string_view>& bannedSubDomains) {
    while (!domain.empty()) {
        if (bannedSubDomains.find(domain) != bannedSubDomains.end()) {
            return true;
        }
        auto pointIt = domain.find('.');
        if (pointIt == domain.npos) {
            break;
        }
        domain.remove_prefix(pointIt + 1);
    }
    return false;
}

vector<bool> checkDomains(const vector<string>& domainsToCheck, const unordered_set<string_view>& bannedSubDomains) {
    vector<bool> logs;
    logs.reserve(domainsToCheck.size());

    for (const auto& domain : domainsToCheck) {
        logs.push_back(!isBanned(domain, bannedSubDomains));
    }
    return logs;
}

void printLogs(ostream& os, const vector<bool>& logs) {
    map<bool, string> answer = {{false, "Bad"}, {true, "Good"}};

    for (const auto& log : logs) {
        os << answer[log] << endl;
    }
}

int main() {
    const vector<string> banned_domains = ReadDomains();
    const vector<string> domains_to_check = ReadDomains();

    const unordered_set<string_view> bannedSubDomains = getSubDomains(banned_domains);

    const vector<bool> logs = checkDomains(domains_to_check, bannedSubDomains);

    printLogs(cout, logs);

    return 0;
}

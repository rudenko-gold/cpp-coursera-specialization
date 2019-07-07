#include "database.h"

void Database::Add(const Date& date, const string& event) {
    auto& events = storage[date];
    
    if (events.addingLog.find(event) == events.addingLog.end()) {
        events.addingLog.insert(event);
        events.orderLog.push_back(event);
    }
}

ostream& Database::Print(ostream& os) const {
    for (const auto& record : storage) {
        for (const auto& event : record.second.orderLog) {
            os << record.first << " " << event << "\n";
        }
    }
    return os;
}

string Database::Last(const Date& d) const {
    auto it = storage.upper_bound(d);
    if (it == storage.begin()) {
        return "No entries";
    }
    else {
        it--;
        ostringstream os;
        os << it->first << " " << it->second.orderLog.back();
        return os.str();
    }
}
#include "date.h"
#include "condition_parser.h"
#include "node.h"
#include "database.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>

using namespace std;

string ParseEvent(istream& is) {
    string result = "";
    getline(is, result);
    for (int i = 0; i < result.size(); i++) {
        if (result[i] != ' ') {
            result.erase(result.begin(), result.begin() + i);
            break;
        }
    }
    return result;
}
void TestParseEvent() {
    {
        istringstream is("event");
        cout << ParseEvent(is) << "==event?" << endl;
    }
    {
        istringstream is("   sport event ");
        cout << ParseEvent(is) << "==sport event " << endl;
    }
    {
        istringstream is("  first event  \n  second event");
        vector<string> events;
        events.push_back(ParseEvent(is));
        events.push_back(ParseEvent(is));
        cout << events[0] << "==first event  " << endl;
        cout << events[1] << "==second event" << endl;
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    Database db;

    for (string line; getline(cin, line); ) {
        istringstream is(line);

        string command;
        is >> command;
        if (command == "Add") {
            const auto date = ParseDate(is);
            const auto event = ParseEvent(is);
            db.Add(date, event);
        }
        else if (command == "Print") {
            db.Print(cout);
        }
        else if (command == "Del") {
            auto condition = ParseCondition(is);
            auto predicate = [condition](const Date& date, const string& event) {
                return condition->Evaluate(date, event);
            };
            int count = db.RemoveIf(predicate);
            cout << "Removed " << count << " entries" << "\n";
        }
        else if (command == "Find") {
            auto condition = ParseCondition(is);
            auto predicate = [condition](const Date& date, const string& event) {
                return condition->Evaluate(date, event);
            };

            const auto entries = db.FindIf(predicate);
            for (const auto& entry : entries) {
                cout << entry << "\n";
            }
            cout << "Found " << entries.size() << " entries" << "\n";
        }
        else if (command == "Last") {
            try {
                cout << db.Last(ParseDate(is)) << "\n";
            }
            catch (invalid_argument&) {
                cout << "No entries" << "\n";
            }
        }
        else if (command.empty()) {
            continue;
        }
        else {
            throw logic_error("Unknown command: " + command);
        }
    }

    return 0;
}

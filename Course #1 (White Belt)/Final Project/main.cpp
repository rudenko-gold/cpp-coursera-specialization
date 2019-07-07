#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <iomanip>
#include <sstream>

using namespace std;

class Date {
public:
    Date(string date) {
        stringstream ss(date);
        int year, month, day;

        ss >> year;
        if (ss.peek() != '-') { 
            throw invalid_argument("Wrong date format: " + date); 
        } else {
            ss.ignore(1);
            ss >> month;
        }
        
        if (ss.peek() != '-') {
            throw invalid_argument("Wrong date format: " + date);
        } else {
            ss.ignore(1);
            if (ss.peek() == '-') {
                ss.ignore(1);
                if (ss.peek() < '0' || ss.peek() > '9') {
                    throw invalid_argument("Wrong date format: " + date);
                }
                ss >> day;
                day *= -1;
            } else if (ss.peek() == '+') {
                ss.ignore(1);
                if (ss.peek() < '0' || ss.peek() > '9') {
                    throw invalid_argument("Wrong date format: " + date);
                }
                ss >> day;
            } else {
                if (ss.peek() < '0' || ss.peek() > '9') {
                    throw invalid_argument("Wrong date format: " + date);
                }
                ss >> day;
            }
            
        }
        string s;
        ss >> s;
        if (s.size() > 0) {
            throw invalid_argument("Wrong date format: " + date);
        }
        if (month < 1 || month > 12) {
            throw invalid_argument("Month value is invalid: " + to_string(month));
        }
        if (day < 1 || day > 31) {
            throw invalid_argument("Day value is invalid: " + to_string(day));
        }

        this->year = year;
        this->month = month;
        this->day = day;
    }

    int GetYear() const {
        return year;
    }
    int GetMonth() const {
        return month;
    }
    int GetDay() const {
        return day;
    }
private:
    int year;
    int month;
    int day;
};

bool operator<(const Date& lhs, const Date& rhs) {
    if (lhs.GetYear() != rhs.GetYear()) {
        return lhs.GetYear() < rhs.GetYear();
    } else {
        if (lhs.GetMonth() != rhs.GetMonth()) {
            return lhs.GetMonth() < rhs.GetMonth();
        } else {
            return lhs.GetDay() < rhs.GetDay();
        }
    }
}

ostream& operator<<(ostream& output, const Date& date) {
    output << setw(4) << setfill('0') << date.GetYear() << "-" << setw(2) <<
        setfill('0') << date.GetMonth() << "-" << setw(2) << setfill('0') << date.GetDay();
    return output;
}

class Database {
public:
    void AddEvent(const Date& date, const string& event) {
        db[date].insert(event);
    }
    bool DeleteEvent(const Date& date, const string& event) {
        if (db.find(date) == db.end()) {
            return false;
        } else {
            if (db[date].find(event) == db[date].end()) {
                return false;
            } else {
                db[date].erase(event);
                return true;
            }
        }
    }
    int DeleteDate(const Date& date) {
        int deletedEvents = 0;
        if (db.find(date) != db.end()) {
            deletedEvents = db[date].size();
            db[date].clear();
        }
        return deletedEvents;
    }

    vector<string> Find(const Date& date) const {
        if (db.find(date) != db.end()) {
            vector<string> result(db.at(date).begin(), db.at(date).end());
            return result;
        }
        return {};
    }

    void Print() const {
        for (const auto& date : db) {
            for (const auto& event : date.second) {
                cout << date.first << " " << event << endl;
            }
        }
    }
private:
    map<Date, set<string>> db;
};

int main() {
    Database db;
    
    string command;
    while (getline(cin, command)) {
        if (command == "") {
            continue;
        }

        stringstream ss(command);
        string operation;
        ss >> operation;
        
        if (operation == "Add") {
            try {
                string date;
                string event;
                ss >> date >> event;
                db.AddEvent(Date(date), event);
            } catch(exception& ex) {
                cout << ex.what() << endl; 
                //return 0;
            }
        } else if (operation == "Find") {
            try {
                string date;
                ss >> date;
                vector<string> events = db.Find(Date(date));
                for (auto event : events) {
                    cout << event << endl;
                }
            } catch (exception& ex) {
                cout << ex.what() << endl;
                return 0;
            }
        } else if (operation == "Del") {
            try {
                string date;
                string event;
                ss >> date >> event;
                if (event.size() > 0) {
                    bool deletingSuccess = db.DeleteEvent(Date(date), event);
                    if (deletingSuccess) {
                        cout << "Deleted successfully" << endl;
                    } else {
                        cout << "Event not found" << endl;
                    }
                } else {
                    int deletedEvents = db.DeleteDate(Date(date));
                    cout << "Deleted " << deletedEvents << " events" << endl;
                }
            }
            catch (exception& ex) {
                cout << ex.what() << endl;
                return 0;
            }
        } else if (operation == "Print") {
            db.Print();
        } else {
            cout << "Unknown command: " << operation << endl;
            return 0;
        }
    }
    return 0;
}

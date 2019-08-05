#include <iostream>
#include <ctime>
#include <sstream>
#include <map>

#include "test_runner.h"

using namespace std;

class Date {
public:
    Date(time_t year, time_t month, time_t day) : year(year), month(month), day(day) {
        if (yearIsLeap()) {
            daysInMonth = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        } else {
            daysInMonth = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        }
    }
    Date(string date) {
        stringstream ss(date);
        ss >> year;
        ss.ignore(1);
        ss >> month;
        ss.ignore(1);
        ss >> day;

        if (yearIsLeap()) {
            daysInMonth = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        } else {
            daysInMonth = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        }
    }

    string toString() const {
        string yearString  = to_string(year);
        string monthString = to_string(month);
        string dayString   = to_string(day);

        if (monthString.size() == 1) {
            monthString = "0" + monthString;
        }
        if (dayString.size() == 1) {
            dayString = "0" + dayString;
        }

        return yearString + "-" + monthString + "-" + dayString;
    }

    time_t asTimestamp() const {
        std::tm t;
        t.tm_sec   = 0;
        t.tm_min   = 0;
        t.tm_hour  = 0;
        t.tm_mday  = day;
        t.tm_mon   = month - 1;
        t.tm_year  = year - 1900;
        t.tm_isdst = 0;
        return mktime(&t);
    }

    time_t Year() const {
        return year;
    }

    time_t Month() const {
        return month;
    }

    time_t Day() const {
        return day;
    }

    bool yearIsLeap() const {
        if (year % 4 != 0) {
            return false;
        } else {
            if (year % 100 == 0 ) {
                if (year % 400 == 0) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return true;
            }
        }
    }

    Date nextDay() const {
        Date nextDay = *this;
        nextDay.day++;
        if (nextDay.day > daysInMonth[nextDay.month - 1]) {
            nextDay.day = 1;
            nextDay.month++;
            if (nextDay.month == 13) {
                nextDay.month = 1;
                nextDay.year++;
            }
        }

        return nextDay;
    }

private:
    vector<int> daysInMonth;
    time_t year;
    time_t month;
    time_t day;
};

class DateHasher {
public:
    size_t operator() (const Date& date) const {
        const size_t x = (1 << 16) - 1;

        return date.Year() * x * x + date.Month() * x + date.Day();
    }
};

bool operator< (const Date& lhs, const Date& rhs) {
    if (lhs.Year() == rhs.Year()) {
        if (lhs.Month() == rhs.Month()) {
            return lhs.Day() < rhs.Day();
        } else {
            return lhs.Month() < rhs.Month();
        }
    } else {
        return lhs.Year() < rhs.Year();
    }
}

bool operator> (const Date& lhs, const Date& rhs) {
    if (lhs.Year() == rhs.Year()) {
        if (lhs.Month() == rhs.Month()) {
            return lhs.Day() > rhs.Day();
        } else {
            return lhs.Month() > rhs.Month();
        }
    } else {
        return lhs.Year() > rhs.Year();
    }
}

bool operator== (const Date& lhs, const Date& rhs) {
    return lhs.Year() == rhs.Year() &&
           lhs.Month() == rhs.Month() &&
           lhs.Day() == rhs.Day();
}

bool operator!= (const Date& lhs, const Date& rhs) {
    return !(lhs == rhs);
}

bool operator<= (const Date& lhs, const Date& rhs) {
    return lhs < rhs || lhs == rhs;
}

bool operator>= (const Date& lhs, const Date& rhs) {
    return lhs > rhs || lhs == rhs;
}

ostream& operator<<(ostream& os, const Date& date) {
    os << date.toString();
    return os;
}

int computeDaysDiff(const Date& date_to, const Date& date_from) {
    const time_t timestamp_to = date_to.asTimestamp();
    const time_t timestamp_from = date_from.asTimestamp();
    static const int SECONDS_IN_DAY = 60 * 60 * 24;
    return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

using Days = time_t;

Days ParseDate(std::string source) {
    static const int SECONDS_IN_DAY = 60 * 60 * 24;
    std::tm t;
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;

    size_t begin = 0;
    size_t end = source.find('-', begin);

    std::string test = source.substr(begin, end - begin);
    t.tm_year = stoi(test);
    t.tm_year -= 1900;

    begin = end + 1;
    end = source.find('-', begin);

    test = source.substr(begin, end - begin);
    t.tm_mon = stoi(source.substr(begin, end));
    t.tm_mon -= 1;

    test = source.substr(end + 1);
    t.tm_mday = stoi(source.substr(end + 1));

    t.tm_isdst = 0;
    return mktime(&t) / SECONDS_IN_DAY + 1;
}

class BudgetManager {
public:
    using Income = long double;

    BudgetManager() {
        income.resize(ParseDate("2101-01-01"));
    }

    Income computeIncome(const Date& from, const Date& to) const {
        Income totalIncome = 0.0;
        const size_t dayFrom = ParseDate(from.toString());
        const size_t dayTo = ParseDate(to.toString());
        for (size_t day = dayFrom; day <= dayTo; ++day) {
            totalIncome += income[day];
        }
        return totalIncome;
    }

    void earn(const Date& from, const Date& to, Income valueForAllDays) {
        const size_t dayFrom = ParseDate(from.toString());
        const size_t dayTo = ParseDate(to.toString());

        Income valuePerDay = valueForAllDays / (dayTo - dayFrom + 1);
        for (size_t day = dayFrom; day <= dayTo; ++day) {
            income[day] += valuePerDay;
        }
    }

    void payTax(Date from, Date to, int persentage = 13) {
        const size_t dayFrom = ParseDate(from.toString());
        const size_t dayTo = ParseDate(to.toString());

        for (size_t day = dayFrom; day <= dayTo; ++day) {
            income[day] *= (1.0 - (persentage / 100.0));
        }
    }

private:
    vector<Income> income;
};

int main() {
    BudgetManager budgetManager;

    size_t queriesSize;
    cin >> queriesSize;

    for (size_t query = 0; query < queriesSize; ++query) {
        string command, fromDate, toDate;
        cin >> command >> fromDate >> toDate;

        if (command == "Earn") {
            double value;
            cin >> value;
            budgetManager.earn(Date(fromDate), Date(toDate), value);
        } else if (command == "ComputeIncome") {
            cout.setf(ios_base::fixed);
            cout << budgetManager.computeIncome(Date(fromDate), Date(toDate)) << endl;
        } else if (command == "PayTax") {
            budgetManager.payTax(Date(fromDate), Date(toDate));
        }
    }

    return 0;
}

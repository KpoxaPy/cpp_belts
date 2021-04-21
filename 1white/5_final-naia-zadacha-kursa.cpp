#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <set>

using namespace std;

const set<string> NO_EVENTS;

class Date {
public:
    Date() {
        year = 1970;
        month = 1;
        day = 1;
    }

    Date(int y, int m, int d) {
        if (m < 1 || 12 < m) {
            throw invalid_argument("Month value is invalid: " + to_string(m));
        }
        if (d < 1 || 31 < d) {
            throw invalid_argument("Day value is invalid: " + to_string(d));
        }
        year = y;
        month = m;
        day = d;
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
    int year = 0;
    int month = 0;
    int day = 0;
};

bool operator<(const Date& lhs, const Date& rhs) {
    if (lhs.GetYear() < rhs.GetYear()) {
        return true;
    }
    if (lhs.GetYear() == rhs.GetYear()
        && lhs.GetMonth() < rhs.GetMonth()) {
        return true;
    }
    if (lhs.GetYear() == rhs.GetYear()
        && lhs.GetMonth() == rhs.GetMonth()
        && lhs.GetDay() < rhs.GetDay()) {
        return true;
    }
    return false;
}

ostream& operator<<(ostream& s, const Date& d) {
    s << setfill('0')
        << setw(4) << d.GetYear() << '-'
        << setw(2) << d.GetMonth() << '-'
        << setw(2) << d.GetDay();    
    return s;
}

istream& operator>>(istream& s, Date& d) {
    int year, month, day;
    char sep1, sep2;
    if (s) {
        s >> year >> sep1 >> month >> sep2 >> day;
        if (s) {
            if (sep1 == '-' && sep2 == '-') {
                d = Date(year, month, day);
            } else {
                s.setstate(ios::failbit);
            }
        }
    }
    return s;
}

Date ParseDate(string s) {
    stringstream ss(s);
    Date d;
    if (ss >> d) {
        ss.get();
        if (ss.eof()) {
            return d;
        }
    }
    throw invalid_argument("Wrong date format: " + s);
}

class Database {
public:
    void AddEvent(const Date& date, const string& event) {
        events[date].insert(event);
    }

    bool DeleteEvent(const Date& date, const string& event) {
        if (auto date_events_it = events.find(date); date_events_it != end(events)) {
            set<string>& date_events = date_events_it->second;
            if (auto event_it = date_events.find(event); event_it != end(date_events)) {
                date_events.erase(event_it);
                return true;
            }
        }
        return false;
    }

    int DeleteDate(const Date& date) {
        if (auto date_events_it = events.find(date); date_events_it != end(events)) {
            int events_num = date_events_it->second.size();
            events.erase(date_events_it);
            return events_num;
        }
        return 0;
    }

    const set<string>& Find(const Date& date) const {
        if (auto date_events_it = events.find(date); date_events_it != end(events)) {
            return date_events_it->second;
        }
        return NO_EVENTS;
    }

    void Print() const {
        for (const auto& [date, date_events] : events) {
            for (const auto& event : date_events) {
                cout << date << " " << event << endl;
            }
        }
    }

private:
    map<Date, set<string>> events;
};

ostream& operator<<(ostream& s, const set<string>& es) {
    for (const auto& e : es) {
        s << e << endl;
    }
    return s;
}

int main() {
    Database db;

    string line;
    while (getline(cin, line)) {
        try {
            if (line.empty()) {
                continue;
            }

            stringstream ss(line);
            string command;
            string date;
            string event;

            ss >> command;
            if (command == "Add") {
                ss >> date >> event;
                Date d = ParseDate(date);
                db.AddEvent(d, event);
            } else if (command == "Del") {
                ss >> date >> event;
                Date d = ParseDate(date);
                if (ss) {
                    if (db.DeleteEvent(d, event)) {
                        cout << "Deleted successfully" << endl;
                    } else {
                        cout << "Event not found" << endl;
                    }
                } else {
                    int n = db.DeleteDate(d);
                    cout << "Deleted " << n << " events" << endl;
                }
            } else if (command == "Find") {
                ss >> date;
                Date d = ParseDate(date);
                cout << db.Find(d);
            } else if (command == "Print") {
                db.Print();
            } else {
                throw invalid_argument("Unknown command: " + command);
            }
        } catch (invalid_argument& ex) {
            cout << ex.what() << endl;
            return 1;
        }
    }

    return 0;
}
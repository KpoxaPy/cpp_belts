#include "date.h"

#include <iomanip>
#include <sstream>

using namespace std;

Date::Date(int y, int m, int d)
{
    if (m < 1 || 12 < m)
    {
        throw invalid_argument("Month value is invalid: " + to_string(m));
    }
    if (d < 1 || 31 < d)
    {
        throw invalid_argument("Day value is invalid: " + to_string(d));
    }
    year_ = y;
    month_ = m;
    day_ = d;
    is_valid_ = true;
}

int Date::GetYear() const
{
    return year_;
}

int Date::GetMonth() const
{
    return month_;
}
int Date::GetDay() const
{
    return day_;
}

bool Date::IsValid() const {
    return is_valid_;
}

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

bool operator>(const Date& lhs, const Date& rhs) {
    return rhs < lhs;
}

bool operator<=(const Date& lhs, const Date& rhs) {
    return !(rhs < lhs);
}

bool operator>=(const Date& lhs, const Date& rhs) {
    return !(lhs < rhs);
}

bool operator==(const Date& lhs, const Date& rhs) {
    return !(lhs < rhs) && !(rhs < lhs);
}

bool operator!=(const Date& lhs, const Date& rhs) {
    return lhs < rhs || rhs < lhs;
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

Date ParseDate(istream& s) {
    string date_arg;
    s >> date_arg;
    stringstream ss(date_arg);
    Date d;
    if (ss >> d) {
        ss.get();
        if (ss.eof()) {
            return d;
        }
    }
    throw invalid_argument("Wrong date format: " + date_arg);
}

#pragma once

#include <iostream>
#include <string>

class Date {
public:
    Date() = default;
    Date(int y, int m, int d);

    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;

    bool IsValid() const;

private:
    int year_ = 0;
    int month_ = 0;
    int day_ = 0;

    bool is_valid_ = false;
};

bool operator<(const Date& lhs, const Date& rhs);
bool operator>(const Date& lhs, const Date& rhs);
bool operator<=(const Date& lhs, const Date& rhs);
bool operator>=(const Date& lhs, const Date& rhs);
bool operator==(const Date& lhs, const Date& rhs);
bool operator!=(const Date& lhs, const Date& rhs);

std::ostream& operator<<(std::ostream& s, const Date& d);
std::istream& operator>>(std::istream& s, Date& d);

Date ParseDate(std::istream& s);
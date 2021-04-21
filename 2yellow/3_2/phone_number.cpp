#include "phone_number.h"

#include <stdexcept>
#include <sstream>

PhoneNumber::PhoneNumber(const string &international_number) {
    istringstream s(international_number);
    char plus, minus1, minus2;
    int country, city;
    string local; 
    if (s) {
        s >> plus >> country >> minus1 >> city >> minus2 >> local; 
        if (s) {
            if (plus == '+' && minus1 == '-' && minus2 == '-') {
                country_code_ = to_string(country);
                city_code_ = to_string(city);
                local_number_ = local;
                return;
            }
        }
    }
    throw invalid_argument("");
}

string PhoneNumber::GetCountryCode() const {
    return country_code_;    
}

string PhoneNumber::GetCityCode() const {
    return city_code_;
}

string PhoneNumber::GetLocalNumber() const {
    return local_number_;
}

string PhoneNumber::GetInternationalNumber() const {
    return "+" + country_code_ + "-" + city_code_ + "-" + local_number_;
}
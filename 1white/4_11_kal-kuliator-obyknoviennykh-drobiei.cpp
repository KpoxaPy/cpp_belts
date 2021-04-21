#include <iostream>
#include <sstream>
#include <numeric>
#include <exception>
using namespace std;

class Rational {
public:
    Rational()
        : numerator_(0), denominator_(1)
    {}

    Rational(int numerator, int denominator)
        : numerator_(numerator), denominator_(denominator)
    {
        if (denominator == 0) {
            throw invalid_argument("Invalid argument");
        }
        Simplify();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

private:
    // Добавьте поля
    int numerator_;
    int denominator_;

    void Simplify() {
        if (auto d = gcd(numerator_, denominator_); d > 1) {
            numerator_ /= d;
            denominator_ /= d;
        }

        if (denominator_ < 0) {
            denominator_ *= -1;
            numerator_ *= -1;
        }
    }
};

Rational operator+(const Rational& lhs, const Rational& rhs) {
    return {lhs.Numerator() * rhs.Denominator() + rhs.Numerator() * lhs.Denominator(), lhs.Denominator() * rhs.Denominator()};
}

Rational operator-(const Rational& lhs, const Rational& rhs) {
    return {lhs.Numerator() * rhs.Denominator() - rhs.Numerator() * lhs.Denominator(), lhs.Denominator() * rhs.Denominator()};
}

Rational operator*(const Rational& lhs, const Rational& rhs) {
    return {lhs.Numerator() * rhs.Numerator(), lhs.Denominator() * rhs.Denominator()};
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
    if (rhs.Numerator() == 0) {
        throw domain_error("Division by zero");
    }
    return {lhs.Numerator() * rhs.Denominator(), lhs.Denominator() * rhs.Numerator()};
}

bool operator==(const Rational& lhs, const Rational& rhs) {
    return lhs.Numerator() == rhs.Numerator() && lhs.Denominator() == rhs.Denominator();
}

bool operator<(const Rational& lhs, const Rational& rhs) {
    return (lhs - rhs).Numerator() < 0;
}

ostream& operator<<(ostream& stream, const Rational& v) {
    stream << v.Numerator() << "/" << v.Denominator();
    return stream;
}

istream& operator>>(istream& stream, Rational& v) {
    int numerator, denominator;
    char c;

    if (stream) {
        stream >> numerator >> c >> denominator;
        if (stream) {
            if (c == '/') {
                v = Rational(numerator, denominator);
            } else {
                stream.setstate(ios_base::failbit);
            }
        }
    }
    return stream;
}

int main() {
    string s;
    while (getline(cin, s)) {
        try {
            if (s.empty()) {
                continue;
            }
            stringstream ss(s);
            Rational lhs, rhs;
            char op;
            ss >> lhs >> op >> rhs;
            Rational result;
            if (op == '+') {
                result = lhs + rhs;
            } else if (op == '-') {
                result = lhs - rhs;
            } else if (op == '*') {
                result = lhs * rhs;
            } else if (op == '/') {
                result = lhs / rhs;
            }
            cout << result << endl;
        } catch (exception& ex) {
            cout << ex.what() << endl;
        }
    }
    return 0;
}
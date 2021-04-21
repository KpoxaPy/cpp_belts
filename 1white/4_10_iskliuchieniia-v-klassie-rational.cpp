#include <iostream>
#include <map>
#include <set>
#include <vector>
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
            throw invalid_argument("Denominator should be nonzero");
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
        throw domain_error("Cannot divide by zero");
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

// Вставьте сюда реализацию operator / для класса Rational

int main() {
    try {
        Rational r(1, 0);
        cout << "Doesn't throw in case of zero denominator" << endl;
        return 1;
    } catch (invalid_argument&) {
    }

    try {
        auto x = Rational(1, 2) / Rational(0, 1);
        cout << "Doesn't throw in case of division by zero" << endl;
        return 2;
    } catch (domain_error&) {
    }

    cout << "OK" << endl;
    return 0;
}
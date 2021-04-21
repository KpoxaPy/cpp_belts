#include "tests.h"
#include "test_runner.h"

#include "phone_number.h"

string w(const string& s) {
    return PhoneNumber(s).GetInternationalNumber();
}

void TestPhones() {
    AssertEqual(w("+7-495-111-22-33"), "+7-495-111-22-33");
    AssertEqual(w("+7-495-1112233"), "+7-495-1112233");
    AssertEqual(w("+323-22-460002"), "+323-22-460002");
    AssertEqual(w("+1-2-coursera-cpp"), "+1-2-coursera-cpp");
    AssertException(w("1-2-333"), invalid_argument, "1-2-333");
    AssertException(w("+7-1233"), invalid_argument, "+7-1233");
}

void TestAll() {
    TestRunner tr;
    tr.RunTest(TestPhones, "TestPhones");
}
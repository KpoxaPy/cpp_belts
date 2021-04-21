// Comment before submit!!!
// #include "good.cpp"

#include "test_runner.h"
#include <sstream>

using namespace std;

void TestPrintStats() {
  ostringstream out;
  AgeStats s = {
    1, 2, 3, 4, 5, 6, 7
  };
  PrintStats(s, out);
  ASSERT_EQUAL(out.str(),
    "Median age = 1\n"
    "Median age for females = 2\n"
    "Median age for males = 3\n"
    "Median age for employed females = 4\n"
    "Median age for unemployed females = 5\n"
    "Median age for employed males = 6\n"
    "Median age for unemployed males = 7\n"
  );
}

void TestComputeMedianAge() {
  {
    vector<Person> v;
    ASSERT_EQUAL(ComputeMedianAge(v.begin(), v.end()), 0);
  }
  {
    vector<Person> v = {{10, Gender::MALE, false}, {70, Gender::MALE, false}, {40, Gender::MALE, true}};
    ASSERT_EQUAL(ComputeMedianAge(v.begin(), v.end()), 40);
  }
}

bool operator==(const AgeStats& lhs, const AgeStats& rhs) {
  return tie(lhs.total, lhs.females, lhs.males, lhs.employed_females, lhs.unemployed_females, lhs.employed_males, lhs.unemployed_males)
    == tie(rhs.total, rhs.females, rhs.males, rhs.employed_females, rhs.unemployed_females, rhs.employed_males, rhs.unemployed_males);
}

ostream& operator<<(ostream& s, const AgeStats& stats) {
  return s << "{"
    << stats.total << ", "
    << stats.females << ", "
    << stats.males << ", "
    << stats.employed_females << ", "
    << stats.unemployed_females << ", "
    << stats.employed_males << ", "
    << stats.unemployed_males << "}";
}

void TestComputeStats() {
  vector<Person> persons = {
    {10, Gender::MALE, false},
    {11, Gender::MALE, false},
    {12, Gender::MALE, true},
    {13, Gender::MALE, true},
    {14, Gender::FEMALE, false},
    {15, Gender::FEMALE, false},
    {16, Gender::FEMALE, true},
    {17, Gender::FEMALE, true}
  };
  AgeStats expected = {
    14, 16, 12, 17, 15, 13, 11
  };
  ASSERT_EQUAL(ComputeStats(persons), expected);
}

void TestRead() {
  istringstream in("4 11 0 0 12 0 1 13 1 0 14 1 1");
  vector<Person> expected = {
    {11, Gender::FEMALE, false},
    {12, Gender::FEMALE, true},
    {13, Gender::MALE, false},
    {14, Gender::MALE, true}
  };
  ASSERT_EQUAL(ReadPersons(in), expected);
}

void TestAll() {
  TestRunner tr;
  // RUN_TEST(tr, Test);
  RUN_TEST(tr, TestPrintStats);
  RUN_TEST(tr, TestComputeMedianAge);
  RUN_TEST(tr, TestComputeStats);
  RUN_TEST(tr, TestRead);
}

int main() {
  TestAll();
  return 0;
}
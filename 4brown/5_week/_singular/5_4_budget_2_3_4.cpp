#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <stdexcept>
#include <array>
#include <map>

#include "test_runner.h"

using namespace std;

template <typename T, typename R1, typename R2 = R1>
void CheckInRange(T value, R1 from, R2 to = numeric_limits<R1>::max()) {
  if (static_cast<T>(from) <= value && value <= static_cast<T>(to)) {
    return;
  }
  ostringstream ss;
  ss << "value " << value << " not in range [" << from << "; " << to << "]";
  throw out_of_range(ss.str());
}

bool IsLeapYear(int year) {
  return (year % 4 == 0) && !(year % 100 == 0 && year % 400 != 0);
}

int CountLeapYearsBefore(int year) {
  return year / 4 - year / 100 + year / 400 - (IsLeapYear(year) ? 1 : 0);
}

class Date {
 public:
  Date(uint16_t y = 2000, uint8_t m = 1, uint8_t d = 1) {
    CheckDateValidity(y, m, d);
    year_ = y;
    month_ = m;
    day_ = d;
  }

  int GetYear() const {
    return year_;
  }

  int GetMonth() const {
    return month_;
  }

  int GetDay() const {
    return day_;
  }

  uint64_t Days() const {
    uint64_t l_days = year_ * 365 + CountLeapYearsBefore(year_) + day_;
    const auto& lengths = IsLeapYear(year_) ? MONTH_LENGTHS_LEAP : MONTH_LENGTHS;
    for (size_t i = 0; i < static_cast<size_t>(month_ - 1); ++i) {
      l_days += lengths[i];
    }
    return l_days;
  }

  static void CheckDateValidity(uint16_t y, uint8_t m, uint8_t d) {
    CheckInRange(y, 1);
    CheckInRange(m, 1, 12);

    if (IsLeapYear(y)) {
      CheckInRange(d, 1, MONTH_LENGTHS_LEAP[m - 1]);
    } else {
      CheckInRange(d, 1, MONTH_LENGTHS[m - 1]);
    }
  }

 private:
  static const array<uint8_t, 12> MONTH_LENGTHS_LEAP;
  static const array<uint8_t, 12> MONTH_LENGTHS;

  uint16_t year_;
  uint8_t month_;
  uint8_t day_;
};

const array<uint8_t, 12> Date::MONTH_LENGTHS_LEAP = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const array<uint8_t, 12> Date::MONTH_LENGTHS = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// returns diff in days
int64_t operator-(Date lhs, Date rhs) {
  return lhs.Days() - rhs.Days();
}

bool operator<(Date lhs, Date rhs) {
  return vector<int>({lhs.GetYear(), lhs.GetMonth(), lhs.GetDay()})
    < vector<int>({rhs.GetYear(), rhs.GetMonth(), rhs.GetDay()});
}

bool operator>(Date lhs, Date rhs) {
  return rhs < lhs;
}

bool operator<=(Date lhs, Date rhs) {
  return !(rhs < lhs);
}

bool operator>=(Date lhs, Date rhs) {
  return !(lhs < rhs);
}

bool operator==(Date lhs, Date rhs) {
  return !(lhs < rhs) && !(rhs < lhs);
}

bool operator!=(Date lhs, Date rhs) {
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

void TestDateDiff() {
  ASSERT_EQUAL(Date(2021, 1, 1) - Date(2021, 1, 1), 0);
  ASSERT_EQUAL(Date(2021, 1, 1) - Date(2020, 1, 1), 366);
  ASSERT_EQUAL(Date(2020, 1, 1) - Date(2021, 1, 1), -366);
  ASSERT_EQUAL(Date(2020, 1, 1) - Date(2019, 1, 1), 365);
  ASSERT_EQUAL(Date(2019, 1, 1) - Date(2020, 1, 1), -365);
  ASSERT_EQUAL(Date(2020, 12, 31) - Date(2020, 1, 1), 365);
  ASSERT_EQUAL(Date(2021, 12, 31) - Date(2021, 1, 1), 364);
  ASSERT_EQUAL(Date(2020, 1, 15) - Date(2020, 1, 1), 14);
  ASSERT_EQUAL(Date(2020, 2, 15) - Date(2020, 1, 1), 45);
  ASSERT_EQUAL(Date(2020, 3, 15) - Date(2020, 1, 1), 74);
  ASSERT_EQUAL(Date(2020, 4, 15) - Date(2020, 1, 1), 105);
  ASSERT_EQUAL(Date(2020, 5, 15) - Date(2020, 1, 1), 135);
  ASSERT_EQUAL(Date(2020, 6, 15) - Date(2020, 1, 1), 166);
  ASSERT_EQUAL(Date(2020, 7, 15) - Date(2020, 1, 1), 196);
  ASSERT_EQUAL(Date(2020, 7, 15) - Date(1970, 1, 15), 18444);
}

namespace Budget {

class Budget {
 private:
  struct DailyStat {
    double income = 0;
    double spent = 0;
  };

 public:
  Budget()
    : stats_(MAX_DAYS)
  {}

  double ComputeIncome(Date from, Date to) const {
    double total = 0;

    const DailyStat* end = &stats_[GetDateShift(to)];
    for (const DailyStat* it = &stats_[GetDateShift(from)]; it <= end; ++it) {
      total += it->income - it->spent;
    }

    return total;
  }

  void Earn(Date from, Date to, int value) {
    double daily_income =  static_cast<double>(value) / (to - from + 1);
    DailyStat* end = &stats_[GetDateShift(to)];
    for (DailyStat* it = &stats_[GetDateShift(from)]; it <= end; ++it) {
      it->income += daily_income;
    }
  }

  void Spend(Date from, Date to, int value) {
    double daily_spent =  static_cast<double>(value) / (to - from + 1);
    DailyStat* end = &stats_[GetDateShift(to)];
    for (DailyStat* it = &stats_[GetDateShift(from)]; it <= end; ++it) {
      it->spent += daily_spent;
    }
  }

  void PayTax(Date from, Date to, int tax) {
    double diff = 1.0 - tax / 100.0;
    DailyStat* end = &stats_[GetDateShift(to)];
    for (DailyStat* it = &stats_[GetDateShift(from)]; it <= end; ++it) {
      it->income *= diff;
    }
  }

 private:
  static const size_t MAX_DAYS = 40'000;

  vector<DailyStat> stats_;

  static size_t GetDateShift(Date d) {
    static const uint64_t base = Date(2000, 1, 1).Days();
    return d.Days() - base;
  }
};

void TestTaxAfterSpend() {
  Budget b;
  b.Earn({2020, 1, 1}, {2020, 1, 1}, 16);
  b.Spend({2020, 1, 1}, {2020, 1, 1}, 16);
  b.PayTax({2020, 1, 1}, {2020, 1, 1}, 50);
  ASSERT_EQUAL(b.ComputeIncome({2000, 1, 1}, {2080, 1, 1}), -8);
}

class Command {
 public:
  Command(ostream& s)
    : os_(s) {}
  virtual ~Command() = default;

  virtual void Do(Budget&) {}
 
 protected:
  ostream& os_;
};
using CommandPtr = shared_ptr<Command>;

class ComputeIncome : public Command {
 public:
  ComputeIncome(istringstream& is, ostream& os)
    : Command(os)
  {
    is >> from_ >> to_;
  }

  void Do(Budget& budget) override {
    os_ << budget.ComputeIncome(from_, to_) << "\n";
  }

 private:
  Date from_;
  Date to_;
};

class Earn : public Command {
 public:
  Earn(istringstream& is, ostream& os)
    : Command(os)
  {
    is >> from_ >> to_ >> value_;
  }

  void Do(Budget& budget) override {
    budget.Earn(from_, to_, value_);
  }

 private:
  Date from_;
  Date to_;
  int value_;
};

class PayTax : public Command {
 public:
  PayTax(istringstream& is, ostream& os)
    : Command(os)
  {
    is >> from_ >> to_ >> tax_;
  }

  void Do(Budget& budget) override {
    budget.PayTax(from_, to_, tax_);
  }

 private:
  Date from_;
  Date to_;
  int tax_;
};

class Spend : public Command {
 public:
  Spend(istringstream& is, ostream& os)
    : Command(os)
  {
    is >> from_ >> to_ >> value_;
  }

  void Do(Budget& budget) override {
    budget.Spend(from_, to_, value_);
  }

 private:
  Date from_;
  Date to_;
  int value_;
};

class Parser {
  friend class Command;

 public:
  Parser(istream& is, ostream& os, Budget& budget)
      : is_(is), os_(os), budget_(budget)
  {
    os.precision(25);
  }

  void ProcessQueries() {
    string startline;
    getline(is_, startline);

    istringstream iss(startline);
    int queries_count;
    iss >> queries_count;

    string line;
    while (queries_count-- > 0) {
      getline(is_, line);
      ParseCommand(move(line))->Do(budget_);
    }
  }

  CommandPtr ParseCommand(string line) {
    istringstream iss(line);
    string command;
    iss >> command;

    if (command == "ComputeIncome") {
      return make_shared<ComputeIncome>(iss, os_);
    } else if (command == "Earn") {
      return make_shared<Earn>(iss, os_);
    } else if (command == "PayTax") {
      return make_shared<PayTax>(iss, os_);
    } else if (command == "Spend") {
      return make_shared<Spend>(iss, os_);
    }

    return make_shared<Command>(os_);
  }

 private:
  istream& is_;
  ostream& os_;
  Budget& budget_;
};

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestDateDiff);
  RUN_TEST(tr, TestTaxAfterSpend);
  // RUN_TEST(tr, Test);
}

}  // namespace Budget

int main() {
  Budget::TestAll();

  ios::sync_with_stdio(false); 
  cin.tie(NULL);
  Budget::Budget budget;
  Budget::Parser parser(cin, cout, budget);
  parser.ProcessQueries();

  return 0;
}
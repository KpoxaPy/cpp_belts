#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <cmath>
#include <functional>

#include "test_runner.h"

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter = " ") {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter = " ") {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

int ConvertToInt(string_view str) {
  // use std::from_chars when available to git rid of string copy
  size_t pos;
  const int result = stoi(string(str), &pos);
  if (pos != str.length()) {
    std::stringstream error;
    error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}

template <typename Number>
void ValidateBounds(Number number_to_check, Number min_value, Number max_value) {
  if (number_to_check < min_value || number_to_check > max_value) {
    std::stringstream error;
    error << number_to_check << " is out of [" << min_value << ", " << max_value << "]";
    throw out_of_range(error.str());
  }
}

class Date {
public:
  static Date FromString(string_view str) {
    const int year = ConvertToInt(ReadToken(str, "-"));
    const int month = ConvertToInt(ReadToken(str, "-"));
    ValidateBounds(month, 1, 12);
    const int day = ConvertToInt(str);
    ValidateBounds(day, 1, 31);
    return {year, month, day};
  }

  // Weird legacy, can't wait for std::chrono::year_month_day
  time_t AsTimestamp() const {
    std::tm t;
    t.tm_sec  = 0;
    t.tm_min  = 0;
    t.tm_hour = 0;
    t.tm_mday = day_;
    t.tm_mon  = month_ - 1;
    t.tm_year = year_ - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
  }

private:
  int year_;
  int month_;
  int day_;

  Date(int year, int month, int day)
      : year_(year), month_(month), day_(day)
  {}
};

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
  const time_t timestamp_to = date_to.AsTimestamp();
  const time_t timestamp_from = date_from.AsTimestamp();
  static constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
  return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = ComputeDaysDiff(END_DATE, START_DATE);

size_t ComputeDayIndex(const Date& date) {
  return ComputeDaysDiff(date, START_DATE);
}

struct MoneyState {
  double earned = 0.0;
  double spent = 0.0;

  double ComputeIncome() const {
    return earned - spent;
  }

  MoneyState& operator+=(const MoneyState& other) {
    earned += other.earned;
    spent += other.spent;
    return *this;
  }
  MoneyState operator+(const MoneyState& other) const {
    return MoneyState(*this) += other;
  }

  MoneyState operator*=(double factor) {
    earned *= factor;
    spent *= factor;
    return *this;
  }
  MoneyState operator*(double factor) const {
    return MoneyState(*this) *= factor;
  }
};

struct Node {
  double factor = 1.0;
  MoneyState add;
  MoneyState values;
};
using Data = vector<Node>;

ostream& operator<<(ostream& s, const Node& n) {
  return s << "[factor=" << n.factor << "; add=(+" << n.add.earned << "-" << n.add.spent << "); values=(+" << n.values.earned << "-" << n.values.spent << ")]";
}

template <typename Apply, typename Fold>
struct Traverser {
  size_t from;
  size_t to;
  Data& data;
  Apply apply;
  Fold fold;

  void Push(size_t idx, size_t left, size_t right) {
    // cerr << *this << ": Push(" << idx << ", " << left << ", " << right << ")" << endl;
    // cerr << "data[" << idx <<"] = " << data[idx] << endl;
    for (size_t i = idx * 2; i <= idx * 2 + 1; ++i) {
      if (i < data.size()) {
        // cerr << "data[" << i <<"] = " << data[i] << endl;
        data[i].factor *= data[idx].factor;
        data[i].add.earned *= data[idx].factor;
        data[i].add += data[idx].add;
        data[i].values.earned *= data[idx].factor;
        data[i].values += data[idx].add * ((right - left) / 2);
        // cerr << "data[" << i <<"] = " << data[i] << endl;
      }
    }
    data[idx].factor = 1;
    data[idx].add = {};
    // cerr << "data[" << idx <<"] = " << data[idx] << endl;
  }

  void Go(size_t idx, size_t left, size_t right) {
    // cerr << *this << ": Go(" << idx << ", " << left << ", " << right << "): Start" << endl;
    if (idx >= data.size() || to <= left || right <= from) {
      return;
    }

    Push(idx, left, right);

    if (from <= left && right <= to) {
      // cerr << *this << ": Go(" << idx << ", " << left << ", " << right << "): Apply" << endl;
      // cerr << "data[" << idx <<"] = " << data[idx] << endl;
      apply(data[idx], left, right);
      // cerr << "data[" << idx <<"] = " << data[idx] << endl;
      return;
    }

    Go(idx * 2, left, (left + right) / 2);
    Go(idx * 2 + 1, (left + right) / 2, right);
    if (fold) {
      // Node empty;
      // const Node& left = idx * 2 < data.size() ? data[idx * 2] : empty;
      // const Node& right = idx * 2 + 1 < data.size() ? data[idx * 2 + 1] : empty;
      // fold(data[idx], left, right);
      // cerr << *this << ": Go(" << idx << ", " << left << ", " << right << "): Fold" << endl;
      // cerr << "data[" << idx <<"] = " << data[idx] << endl;
      fold(data[idx], data[idx * 2], data[idx * 2 + 1]);
      // cerr << "data[" << idx <<"] = " << data[idx] << endl;
    }
  }
};

template <typename Apply, typename Fold>
ostream& operator<<(ostream& s, const Traverser<Apply, Fold>& t) {
  return s << "Traverser[" << t.from << ":" << t. to << "]";
}

class Budget {
 public:
  Budget(size_t days_count)
    : days_count_(pow(2, ceil(log2(days_count))))
  {
    size_t s = days_count_ * 2;
    data_.resize(s);
  }

  template <typename Apply, typename Fold=function<void(Node&, const Node&, const Node&)>>
  void Traverse(size_t from, size_t to, Apply apply, Fold fold = {}) {
    Traverser<Apply, Fold>{from, to, data_, move(apply), move(fold)}.Go(1, 0, days_count_);
  }

  double ComputeSum(size_t from, size_t to) {
    double total = 0;
    Traverse(from, to, [&total](Node& n, size_t left, size_t right) mutable {
      total += n.values.ComputeIncome();
    });
    return total;
  }
  
  void Add(size_t from, size_t to, MoneyState value) {
    Traverse(from, to, [value](Node& n, size_t left, size_t right) {
      n.add += value;
      n.values += value * (right - left);
    }, [](Node& parent, const Node& left, const Node& right) {
      parent.values = left.values + right.values;
    });
  }

  void Multiply(size_t from, size_t to, double value) {
    Traverse(from, to, [value](Node& n, size_t, size_t) {
      n.factor *= value;
      n.add.earned *= value;
      n.values.earned *= value;
    }, [](Node& parent, const Node& left, const Node& right) {
      parent.values = left.values + right.values;
    });
  }

 private:
  const size_t days_count_;
  Data data_;
};

void TestEarn(Budget& b, const string& from, const string& to, double value) {
  auto idx_from = ComputeDayIndex(Date::FromString(from));
  auto idx_to = ComputeDayIndex(Date::FromString(to)) + 1;
  b.Add(idx_from, idx_to, {value / (idx_to - idx_from), 0});
}

void TestSpend(Budget& b, const string& from, const string& to, double value) {
  auto idx_from = ComputeDayIndex(Date::FromString(from));
  auto idx_to = ComputeDayIndex(Date::FromString(to)) + 1;
  b.Add(idx_from, idx_to, {0, value / (idx_to - idx_from)});
}

void TestPayTax(Budget& b, const string& from, const string& to, double value) {
  auto idx_from = ComputeDayIndex(Date::FromString(from));
  auto idx_to = ComputeDayIndex(Date::FromString(to)) + 1;
  b.Multiply(idx_from, idx_to, 1 - value / 100);
}

double TestCompute(Budget& b, const string& from, const string& to) {
  auto idx_from = ComputeDayIndex(Date::FromString(from));
  auto idx_to = ComputeDayIndex(Date::FromString(to)) + 1;
  return b.ComputeSum(idx_from, idx_to);
}

void TestSimple() {
  Budget b(DAY_COUNT);
  TestEarn(b, "2020-7-15", "2020-7-15", 32);
  ASSERT_EQUAL(TestCompute(b, "2020-1-1", "2021-1-1"), 32);
  TestPayTax(b, "2020-7-15", "2020-7-15", 50);
  ASSERT_EQUAL(TestCompute(b, "2020-1-1", "2021-1-1"), 16);
  TestSpend(b, "2020-7-15", "2020-7-15", 16);
  ASSERT_EQUAL(TestCompute(b, "2020-1-1", "2021-1-1"), 0);
}

void TestPeriods() {
  Budget b(DAY_COUNT);
  TestEarn(b, "2020-12-31", "2021-1-1", 64);
  ASSERT_EQUAL(TestCompute(b, "2020-1-1", "2020-12-31"), 32);
  TestPayTax(b, "2020-12-31", "2021-1-1", 50);
  ASSERT_EQUAL(TestCompute(b, "2020-1-1", "2020-12-31"), 16);
  TestSpend(b, "2020-12-31", "2021-1-1", 32);
  ASSERT_EQUAL(TestCompute(b, "2020-1-1", "2020-12-31"), 0);
}

void TestTaxAfterSpend() {
  Budget b(DAY_COUNT);
  TestEarn(b, "2020-1-1", "2020-1-1", 16);
  TestSpend(b, "2020-1-1", "2020-1-1", 16);
  TestPayTax(b, "2020-1-1", "2020-1-1", 50);
  ASSERT_EQUAL(TestCompute(b, "2020-1-1", "2020-1-1"), -8);
}

void TestYear() {
  Budget b(DAY_COUNT);
  TestEarn(b, "2020-1-1", "2021-1-1", 16*367);
  ASSERT_EQUAL(TestCompute(b, "2020-1-1", "2030-1-1"), 16*367);
}

void TestCsera2() {
  Budget b(DAY_COUNT);
  TestEarn(b, "2000-01-02", "2000-01-06", 20);
  ASSERT_EQUAL(TestCompute(b, "2000-01-01", "2001-01-01"), 20);
  TestPayTax(b, "2000-01-02", "2000-01-03", 13);
  ASSERT_EQUAL(TestCompute(b, "2000-01-01", "2001-01-01"), 18.96000000000000085265128);
  TestSpend(b, "2000-12-30", "2001-01-02", 14);
  ASSERT_EQUAL(TestCompute(b, "2000-01-01", "2001-01-01"), 8.460000000000000852651283);
  TestPayTax(b, "2000-12-30", "2000-12-30", 13);
  ASSERT_EQUAL(TestCompute(b, "2000-01-01", "2001-01-01"), 8.460000000000000852651283);
}

void TestCrseraTest5() {
  Budget b(DAY_COUNT);
  TestEarn(b, "2001-02-09", "2001-02-15", 711420);
  TestEarn(b, "2001-02-07", "2001-02-13", 686065);
  ASSERT_EQUAL(TestCompute(b, "2001-02-03", "2001-02-24"), 1397485.000000000232830644);

  TestEarn(b, "2001-02-15", "2001-02-22", 893338);
  ASSERT_EQUAL(TestCompute(b, "2001-02-14", "2001-02-21"), 984933.6071428571594879031);
  ASSERT_EQUAL(TestCompute(b, "2001-02-10", "2001-02-22"), 1895163.714285714086145163);

  /*
6
Earn 2001-02-09 2001-02-15 711420
Earn 2001-02-07 2001-02-13 686065
ComputeIncome 2001-02-03 2001-02-24
Earn 2001-02-15 2001-02-22 893338
ComputeIncome 2001-02-14 2001-02-21
ComputeIncome 2001-02-10 2001-02-22

  Correct output:
  1397485.000000000232830644
  984933.6071428571594879031
  1895163.714285714086145163
  */
}

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestSimple);
  RUN_TEST(tr, TestPeriods);
  RUN_TEST(tr, TestTaxAfterSpend);
  RUN_TEST(tr, TestYear);
  RUN_TEST(tr, TestCsera2);
  RUN_TEST(tr, TestCrseraTest5);
  // RUN_TEST(tr, Test);
}

int main() {
  // TestAll();

  cout.precision(25);
  ios::sync_with_stdio(false); 
  cin.tie(NULL);

  Budget budget(DAY_COUNT);

  int q;
  cin >> q;

  for (int i = 0; i < q; ++i) {
    string query_type;
    cin >> query_type;

    string date_from_str, date_to_str;
    cin >> date_from_str >> date_to_str;

    auto idx_from = ComputeDayIndex(Date::FromString(date_from_str));
    auto idx_to = ComputeDayIndex(Date::FromString(date_to_str)) + 1;

    if (query_type == "ComputeIncome") {
      // cerr << "ComputeIncome " << date_from_str << " " << date_to_str << endl;
      cout << budget.ComputeSum(idx_from, idx_to) << endl;
    } else if (query_type == "PayTax") {
      double value;
      cin >> value;
      // cerr << "PayTax " << date_from_str << " " << date_to_str << " " << value << endl;
      budget.Multiply(idx_from, idx_to, 1.0 - value / 100.0);
    } else if (query_type == "Earn") {
      double value;
      cin >> value;
      // cerr << "Earn " << date_from_str << " " << date_to_str << " " << value << endl;
      budget.Add(idx_from, idx_to, {value / (idx_to - idx_from), 0.0});
    } else if (query_type == "Spend") {
      double value;
      cin >> value;
      // cerr << "Spend " << date_from_str << " " << date_to_str << " " << value << endl;
      budget.Add(idx_from, idx_to, {0.0, value / (idx_to - idx_from)});
    }
  }

  return 0;
}

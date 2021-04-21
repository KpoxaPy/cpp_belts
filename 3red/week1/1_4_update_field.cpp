#include "airline_ticket.h"
#include "test_runner.h"

using namespace std;

bool operator<(const Date& lhs, const Date& rhs) {
  return tie(lhs.year, lhs.month, lhs.day) < tie(rhs.year, rhs.month, rhs.day);
}

bool operator==(const Date& lhs, const Date& rhs) {
  return tie(lhs.year, lhs.month, lhs.day) == tie(rhs.year, rhs.month, rhs.day);
}

ostream& operator<<(ostream& s, const Date& d) {
  return s << d.year << "-" << d.month << "-" << d.day;
}

istream& operator>>(istream& s, Date& d) {
  int year, month, day;
  char sep1, sep2;
  if (s) {
    s >> year >> sep1 >> month >> sep2 >> day;
    if (s) {
      if (sep1 == '-' && sep2 == '-') {
        d = {year, month, day};
      } else {
        s.setstate(ios::failbit);
      }
    }
  }
  return s;
}

bool operator<(const Time& lhs, const Time& rhs) {
  return tie(lhs.hours, lhs.minutes) < tie(rhs.hours, rhs.minutes);
}

bool operator==(const Time& lhs, const Time& rhs) {
  return tie(lhs.hours, lhs.minutes) == tie(rhs.hours, rhs.minutes);
}

ostream& operator<<(ostream& s, const Time& t) {
  return s << t.hours << ":" << t.minutes;
}

istream& operator>>(istream& s, Time& t) {
  int h, m;
  char c;
  if (s) {
    s >> h >> c >> m;
    if (s) {
      if (c == ':') {
        t = {h, m};
      } else {
        s.setstate(ios::failbit);
      }
    }
  }
  return s;
}

#define UPDATE_FIELD(ticket, field, values) \
{                                           \
  auto it = values.find(#field);            \
  if (it != values.end()) {                 \
    istringstream is(it->second);           \
    is >> ticket.field;                     \
  }                                         \
}

void TestUpdate() {
  AirlineTicket t;
  t.price = 0;

  const map<string, string> updates1 = {
    {"departure_date", "2018-2-28"},
    {"departure_time", "17:40"},
  };
  UPDATE_FIELD(t, departure_date, updates1);
  UPDATE_FIELD(t, departure_time, updates1);
  UPDATE_FIELD(t, price, updates1);

  ASSERT_EQUAL(t.departure_date, (Date{2018, 2, 28}));
  ASSERT_EQUAL(t.departure_time, (Time{17, 40}));
  ASSERT_EQUAL(t.price, 0);

  const map<string, string> updates2 = {
    {"price", "12550"},
    {"arrival_time", "20:33"},
  };
  UPDATE_FIELD(t, departure_date, updates2);
  UPDATE_FIELD(t, departure_time, updates2);
  UPDATE_FIELD(t, arrival_time, updates2);
  UPDATE_FIELD(t, price, updates2);

  // updates2 не содержит ключей "departure_date" и "departure_time", поэтому
  // значения этих полей не должны измениться
  ASSERT_EQUAL(t.departure_date, (Date{2018, 2, 28}));
  ASSERT_EQUAL(t.departure_time, (Time{17, 40}));
  ASSERT_EQUAL(t.price, 12550);
  ASSERT_EQUAL(t.arrival_time, (Time{20, 33}));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestUpdate);
}

#include "profile.h"

#include <locale>

using namespace std;
using namespace std::chrono;

struct dig_groups : std::numpunct<char> {
  char do_thousands_sep() const {
    return '\'';
  }
  std::string do_grouping() const {
    return "\3";
  }
};

ostream& operator<<(ostream& s, const nanoseconds& us) {
  auto loc = s.getloc();
  s.imbue(std::locale(loc, new dig_groups));
  s << us.count() << "ns";
  s.imbue(loc);
  return s;
}

ostream& operator<<(ostream& s, const microseconds& us) {
  auto loc = s.getloc();
  s.imbue(std::locale(loc, new dig_groups));
  s << us.count() << "us";
  s.imbue(loc);
  return s;
}

ostream& operator<<(ostream& s, const milliseconds& us) {
  auto loc = s.getloc();
  s.imbue(std::locale(loc, new dig_groups));
  s << us.count() << "ms";
  s.imbue(loc);
  return s;
}

ostream& operator<<(ostream& s, const seconds& us) {
  auto loc = s.getloc();
  s.imbue(std::locale(loc, new dig_groups));
  s << us.count() << "s";
  s.imbue(loc);
  return s;
}
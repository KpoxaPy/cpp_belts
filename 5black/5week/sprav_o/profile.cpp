#include "profile.h"

using namespace std;
using namespace std::chrono;

ostream& operator<<(ostream& s, const nanoseconds& us) {
  return s << us.count() << "ns";
}

ostream& operator<<(ostream& s, const microseconds& us) {
  return s << us.count() << "us";
}

ostream& operator<<(ostream& s, const milliseconds& us) {
  return s << us.count() << "ms";
}

ostream& operator<<(ostream& s, const seconds& us) {
  return s << us.count() << "s";
}
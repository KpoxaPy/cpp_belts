#include "responses.h"

using namespace std;

ostream& operator << (ostream& os, const BusesForStopResponse& r) {
  if (r.buses.empty()) {
    os << "No stop";
  } else {
    for (const string& bus : r.buses) {
      os << bus << " ";
    }
  }
  return os;
}

ostream& operator << (ostream& os, const StopsForBusResponse& r) {
  if (r.stops.empty()) {
    os << "No bus";
  } else {
    for (const auto& [stop, buses] : r.stops) {
      cout << "Stop " << stop << ": ";
      if (buses.empty()) {
        cout << "no interchange";
      } else {
        for (const string& bus : buses) {
          cout << bus << " ";
        }
      }
      cout << endl;
    }
  }
  return os;
}

ostream& operator << (ostream& os, const AllBusesResponse& r) {
  if (r.buses.empty()) {
    cout << "No buses" << endl;
  } else {
    for (const auto& [bus, stops] : r.buses) {
      cout << "Bus " << bus << ": ";
      for (const string& stop : stops) {
        cout << stop << " ";
      }
      cout << endl;
    }
  }
 return os;
}

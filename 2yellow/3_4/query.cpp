#include "query.h"

using namespace std;

istream& operator >> (istream& is, Query& q) {
  string op;
  is >> op;
  if (op == "NEW_BUS") {
    q.type = QueryType::NewBus;
    cin >> q.bus;
    int stop_count;
    cin >> stop_count;
    q.stops.clear();
    q.stops.resize(stop_count);
    for (string& stop : q.stops) {
      cin >> stop;
    }
  } else if (op == "BUSES_FOR_STOP") {
    q.type = QueryType::BusesForStop;
    cin >> q.stop;
  } else if (op == "STOPS_FOR_BUS") {
    q.type = QueryType::StopsForBus;
    cin >> q.bus;
  } else if (op == "ALL_BUSES") {
    q.type = QueryType::AllBuses;
  }

  return is;
}

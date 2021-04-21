#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <map>

using namespace std;

enum class QueryType {
  NewBus,
  BusesForStop,
  StopsForBus,
  AllBuses
};

struct Query {
  QueryType type;
  string bus;
  string stop;
  vector<string> stops;
};

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

struct BusesForStopResponse {
  vector<string> buses;
};

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

struct StopsForBusResponse {
  vector<pair<string, vector<string>>> stops;
};

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

struct AllBusesResponse {
  map<string, vector<string>> buses;
};

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

class BusManager {
public:
  void AddBus(const string& bus, const vector<string>& stops) {
    buses_to_stops_[bus] = stops;
    for (const auto& stop : stops) {
      stops_to_buses_[stop].push_back(bus);
    }
  }

  BusesForStopResponse GetBusesForStop(const string& stop) const {
    if (auto it = stops_to_buses_.find(stop); it != end(stops_to_buses_)) {
      return {it->second};
    }
    return {};
  }

  StopsForBusResponse GetStopsForBus(const string& bus) const {
    StopsForBusResponse r;
    if (auto it = buses_to_stops_.find(bus); it != end(buses_to_stops_)) {
      for (const auto& stop : it->second) {
        vector<string> buses;
        for (const string& other_bus : stops_to_buses_.at(stop)) {
          if (bus != other_bus) {
            buses.push_back(other_bus);
          }
        }
        r.stops.emplace_back(stop, buses);
      }
    }
    return r;
  }

  AllBusesResponse GetAllBuses() const {
    return {buses_to_stops_};
  }
private:
  map<string, vector<string>> buses_to_stops_;
  map<string, vector<string>> stops_to_buses_;
};

int main() {
  int query_count;
  Query q;

  cin >> query_count;

  BusManager bm;
  for (int i = 0; i < query_count; ++i) {
    cin >> q;
    switch (q.type) {
    case QueryType::NewBus:
      bm.AddBus(q.bus, q.stops);
      break;
    case QueryType::BusesForStop:
      cout << bm.GetBusesForStop(q.stop) << endl;
      break;
    case QueryType::StopsForBus:
      cout << bm.GetStopsForBus(q.bus) << endl;
      break;
    case QueryType::AllBuses:
      cout << bm.GetAllBuses() << endl;
      break;
    }
  }

  return 0;
}

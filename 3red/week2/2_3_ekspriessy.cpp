#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <set>

using namespace std;

class RouteManager {
public:
  void AddRoute(int start, int finish) {
    reachable_lists_[start].insert(finish);
    reachable_lists_[finish].insert(start);
  }

  int FindNearestFinish(int start, int finish) const {
    int result = abs(start - finish);

    const auto map_it = reachable_lists_.find(start);
    if (map_it == reachable_lists_.end()) {
        return result;
    }

    const set<int>& reachable_stations = map_it->second;
    if (!reachable_stations.empty()) {
      auto it = reachable_stations.lower_bound(finish);
      if (it != reachable_stations.end()) {
        result = min(result, abs(*it - finish));
      }
      if (it != reachable_stations.begin()) {
        --it;
        result = min(result, abs(*it - finish));
      }
    }
    return result;
  }

private:
  map<int, set<int>> reachable_lists_;
};


int main() {
  RouteManager routes;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int start, finish;
    cin >> start >> finish;
    if (query_type == "ADD") {
      routes.AddRoute(start, finish);
    } else if (query_type == "GO") {
      cout << routes.FindNearestFinish(start, finish) << "\n";
    }
  }

  return 0;
}

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
using namespace std;

// enum class Lang {
//     DE, FR, IT
// };

// struct Region {
//     string std_name;
//     string parent_std_name;
//     map<Lang, string> names;
//     int64_t population;
// };

auto get_rank(const Region& r) {
    return tie(r.std_name, r.parent_std_name, r.names, r.population);
}

int FindMaxRepetitionCount(const vector<Region>& regions) {
    map<tuple<const string&, const string&, const map<Lang, string>&, const int64_t&>, int> rc; 
    int max = 0;
    for (const auto& r : regions) {
        int c = ++rc[get_rank(r)];
        if (c > max) {
            max = c;
        }
    }
    return max;
}

// int main() {
//   cout << FindMaxRepetitionCount({
//       {
//           "Moscow",
//           "Russia",
//           {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
//           89
//       }, {
//           "Russia",
//           "Eurasia",
//           {{Lang::DE, "Russland"}, {Lang::FR, "Russie"}, {Lang::IT, "Russia"}},
//           89
//       }, {
//           "Moscow",
//           "Russia",
//           {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
//           89
//       }, {
//           "Moscow",
//           "Russia",
//           {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
//           89
//       }, {
//           "Russia",
//           "Eurasia",
//           {{Lang::DE, "Russland"}, {Lang::FR, "Russie"}, {Lang::IT, "Russia"}},
//           89
//       },
//   }) << endl;
  
//   cout << FindMaxRepetitionCount({
//       {
//           "Moscow",
//           "Russia",
//           {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
//           89
//       }, {
//           "Russia",
//           "Eurasia",
//           {{Lang::DE, "Russland"}, {Lang::FR, "Russie"}, {Lang::IT, "Russia"}},
//           89
//       }, {
//           "Moscow",
//           "Russia",
//           {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou deux"}, {Lang::IT, "Mosca"}},
//           89
//       }, {
//           "Moscow",
//           "Toulouse",
//           {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
//           89
//       }, {
//           "Moscow",
//           "Russia",
//           {{Lang::DE, "Moskau"}, {Lang::FR, "Moscou"}, {Lang::IT, "Mosca"}},
//           31
//       },
//   }) << endl;
  
//   return 0;
// }

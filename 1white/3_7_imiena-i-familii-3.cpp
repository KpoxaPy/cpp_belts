#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

class Person {
public:
  Person(const string& first_name, const string& last_name, int year) {
    first_name_history[year] = first_name;
    last_name_history[year] = last_name;
    birth_year = year;
  }

  void ChangeFirstName(int year, const string& first_name) {
    if (year < birth_year) {
        return;
    }
    first_name_history[year] = first_name;
  }

  void ChangeLastName(int year, const string& last_name) {
    if (year < birth_year) {
        return;
    }
    last_name_history[year] = last_name;
  }

  string GetFullName(int year) const {
    if (year < birth_year) {
        return "No person";
    }

    auto first_name = GetActualName(first_name_history, year);
    auto last_name = GetActualName(last_name_history, year);

    if (first_name.empty() && last_name.empty()) {
        return "Incognito";
    } else if (first_name.empty()) {
        return last_name + " with unknown first name";
    } else if (last_name.empty()) {
        return first_name + " with unknown last name";
    }
    return first_name + " " + last_name;
  }

  string GetFullNameWithHistory(int year) const {
    if (year < birth_year) {
        return "No person";
    }

    auto first_name = GetNameWithHistory(first_name_history, year);
    auto last_name = GetNameWithHistory(last_name_history, year);

    if (first_name.empty() && last_name.empty()) {
        return "Incognito";
    } else if (first_name.empty()) {
        return last_name + " with unknown first name";
    } else if (last_name.empty()) {
        return first_name + " with unknown last name";
    }
    return first_name + " " + last_name;
  }

private:
  map<int, string> first_name_history;
  map<int, string> last_name_history;
  int birth_year;

  string GetActualName(const map<int, string>& name_history, int year) const {
    auto it = name_history.upper_bound(year);
    if (it == cbegin(name_history)) {
        return "";
    }
    return (--it)->second;
  }

  string GetNameWithHistory(const map<int, string>& name_history, int year) const {
    auto it = make_reverse_iterator(name_history.upper_bound(year));
    if (it == crend(name_history)) {
        return "";
    }

    string name = it->second;
    vector<string> names;
    string prev_name = name;
    for (it++; it != crend(name_history); it++) {
        if (it->second != prev_name) {
            names.push_back(it->second);
            prev_name = it->second;
        }
    }
    
    if (names.empty()) {
        return name;
    }

    name += " (";
    bool isFirstMet = false;
    for (const auto& n : names) {
        if (isFirstMet) {
            name += ", ";
        } else {
            isFirstMet = true;
        }
        name += n;
    }
    name += ")";
    return name;
  }
};

// int main() {
//   Person person("Polina", "Sergeeva", 1960);
//   for (int year : {1959, 1960}) {
//     cout << person.GetFullNameWithHistory(year) << endl;
//   }
  
//   person.ChangeFirstName(1965, "Appolinaria");
//   person.ChangeLastName(1967, "Ivanova");
//   for (int year : {1965, 1967}) {
//     cout << person.GetFullNameWithHistory(year) << endl;
//   }

//   return 0;
// }

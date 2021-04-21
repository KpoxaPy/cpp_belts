#include <iostream>
#include <map>
#include <algorithm>
#include <string>

using namespace std;

class Person {
public:
  void ChangeFirstName(int year, const string& first_name) {
    first_name_history[year] = first_name;
  }

  void ChangeLastName(int year, const string& last_name) {
    last_name_history[year] = last_name;
  }

  string GetFullName(int year) {
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

private:
  map<int, string> first_name_history;
  map<int, string> last_name_history;

  string GetActualName(const map<int, string>& name_history, int year) {
    auto it = name_history.upper_bound(year);
    if (it == cbegin(name_history)) {
        return "";
    }
    return (--it)->second;
  }
};



// int main() {
//   Person person;
  
//   person.ChangeFirstName(1965, "Polina");
//   person.ChangeLastName(1967, "Sergeeva");
//   for (int year : {1900, 1965, 1990}) {
//     cout << "In year " << year << " person was " << person.GetFullName(year) << endl;
//   }
  
//   person.ChangeFirstName(1970, "Appolinaria");
//   for (int year : {1969, 1970}) {
//     cout << "In year " << year << " person was " << person.GetFullName(year) << endl;
//   }
  
//   person.ChangeLastName(1968, "Volkova");
//   for (int year : {1969, 1970}) {
//     cout << "In year " << year << " person was " << person.GetFullName(year) << endl;
//   }
  
//   return 0;
// }

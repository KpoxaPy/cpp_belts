#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <numeric>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
  IteratorRange(Iterator begin, Iterator end)
    : first(begin)
    , last(end)
  {
  }

  Iterator begin() const {
    return first;
  }

  Iterator end() const {
    return last;
  }

private:
  Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
  return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
  string name;
  int age, income;
  bool is_male;
};

ostream& operator<<(ostream& s, const Person& p) {
  return s << p.name << ' ' << p.age << ' ' << p.income << ' ' << (p.is_male ? 'M' : 'W');
}

string GetMaxName(const map<string, size_t>& dict) {
  size_t max_count = 0;
  const string* max_name = nullptr;

  for (const auto& [name, count] : dict) {
    if (count > max_count) {
      max_count = count;
      max_name = &name;
    }
  }

  return max_name ? *max_name : "";
}

class PeopleStats {
private:
  struct NameStatPart {
    string name;
    bool is_male;
  };

public:
  explicit PeopleStats(istream& input) {
    int count;
    input >> count;

    map<string, size_t> m_names;
    map<string, size_t> w_names;
    vector<int> ages;

    people.resize(count);
    income_idx.resize(count);
    ages.resize(count);
    for (size_t i = 0; i < people.size(); ++i) {
      Person& p = people[i];
      char gender;
      input >> p.name >> p.age >> p.income >> gender;
      p.is_male = gender == 'M';

      ages[i] = p.age;
      income_idx[i] = p.income;
      if (p.is_male) {
        ++m_names[p.name];
      } else {
        ++w_names[p.name];
      }
    }

    sort(begin(ages), end(ages));
    size_t mature_count = ages.size();
    for (size_t i = 0; i < ages.size(); ++i, --mature_count) {
      age_idx.emplace(ages[i], mature_count);
    }

    sort(begin(income_idx), end(income_idx), [this](int lhs, int rhs) {
      return lhs > rhs;
    });
    for (size_t i = 1; i < income_idx.size(); ++i) {
      income_idx[i] += income_idx[i - 1];
    }

    m_pop_name = GetMaxName(m_names);
    w_pop_name = GetMaxName(w_names);
  }

  void AgeStat(int age) const {
    size_t count = 0; 
    if (auto it = age_idx.lower_bound(age); it != age_idx.end()) {
      count = it->second;
    }

    cout << "There are " << count << " adult people for maturity age " << age << '\n';
  }

  void WealthyCount(int top) const {
    int income = 0;
    if (top > 0) {
      size_t ttop = min(static_cast<size_t>(top - 1), income_idx.size() - 1);
      income = income_idx[ttop];
    }
    cout << "Top-" << top << " people have total income " << income << '\n';
  }

  void PopularNameStat(char gender) const {
    const string& name = [this, gender]{
      if (gender == 'M') {
        return m_pop_name;
      }
      return w_pop_name;
    }();

    if (name.empty()) {
      cout << "No people of gender " << gender << '\n';
    } else {
      cout << "Most popular name among people of gender " << gender << " is "
        << name << '\n';
    }
  }

private:
  vector<Person> people;

  map<int, size_t> age_idx;
  vector<int> income_idx;
  string m_pop_name;
  string w_pop_name;
};

int main() {
  const PeopleStats people(cin);

  for (string command; cin >> command; ) {
    if (command == "AGE") {
      int adult_age;
      cin >> adult_age;

      people.AgeStat(adult_age);
    } else if (command == "WEALTHY") {
      int count;
      cin >> count;

      people.WealthyCount(count);
    } else if (command == "POPULAR_NAME") {
      char gender;
      cin >> gender;

      people.PopularNameStat(gender);
    }
  }
}

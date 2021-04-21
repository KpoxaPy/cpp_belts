#include <set>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

vector<string> SplitIntoWords(const string& s) {
    vector<string> res;
    auto start = s.begin();
    while (true) {
        auto next = find(start, s.end(), ' ');
        res.push_back(string(start, next));
        if (next == s.end()) {
            break;
        }
        start = next + 1;
    }
    return res;
}

int main() {
  string s = "C Cpp Java Python";

  vector<string> words = SplitIntoWords(s);
  cout << words.size() << " ";
  for (auto it = begin(words); it != end(words); ++it) {
    if (it != begin(words)) {
      cout << "/";
    }
    cout << *it;
  }
  cout << endl;
  
  return 0;
}

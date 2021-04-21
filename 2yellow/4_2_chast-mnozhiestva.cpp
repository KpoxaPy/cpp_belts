#include <set>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

template <typename T>
vector<T> FindGreaterElements(const set<T>& elements, const T& border) {
    auto it = find_if(begin(elements), end(elements), [&border](const auto& x) { return x > border; });
    vector<T> res;
    for (; it != end(elements); ++it) {
        res.push_back(*it);
    }
    return res;
}

int main() {
  for (int x : FindGreaterElements(set<int>{1, 5, 7, 8}, 5)) {
    cout << x << " ";
  }
  cout << endl;
  
  string to_find = "Python";
  cout << FindGreaterElements(set<string>{"C", "C++"}, to_find).size() << endl;
  return 0;
}

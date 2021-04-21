#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>

using namespace std;

set<int>::const_iterator FindNearestElement(const set<int>& numbers, int border) {
    auto r = numbers.lower_bound(border);
    if (r == begin(numbers)) {
        return r;
    }
    auto l = prev(r);
    if (r == end(numbers)) {
        return l;
    }
    if (border - *l <= *r - border) {
        return l;
    }
    return r;
}

int main() {
  set<int> numbers = {1, 4, 6};
  cout <<
      *FindNearestElement(numbers, 0) << " " <<
      *FindNearestElement(numbers, 3) << " " <<
      *FindNearestElement(numbers, 5) << " " <<
      *FindNearestElement(numbers, 6) << " " <<
      *FindNearestElement(numbers, 100) << endl;
      
  set<int> empty_set;
  
  cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;
  return 0;
}

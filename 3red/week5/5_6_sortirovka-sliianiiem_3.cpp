#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    if (range_end - range_begin <= 1) {
        return;
    }
    vector<typename RandomIt::value_type> e(
       make_move_iterator(range_begin),
       make_move_iterator(range_end));
    RandomIt p1 = begin(e) + max(e.size() / 3, static_cast<size_t>(1));
    RandomIt p2 = p1 + max(e.size() / 3, static_cast<size_t>(1));
    MergeSort(begin(e), p1);
    MergeSort(p1, p2);
    vector<typename RandomIt::value_type> t;
    merge(
      make_move_iterator(begin(e)), 
      make_move_iterator(p1),
      make_move_iterator(p1),
      make_move_iterator(p2),
      back_inserter(t));

    MergeSort(p2, end(e));
    merge(
      make_move_iterator(begin(t)),
      make_move_iterator(end(t)),
      make_move_iterator(p2),
      make_move_iterator(end(e)),
      range_begin);
}

int main() {
    vector<int> v = {6, 4, 7, 6, 4, 4, 0, 1};
    MergeSort(begin(v), end(v));
    for (int x : v) {
        cout << x << " ";
    }
    cout << endl;
    return 0;
}

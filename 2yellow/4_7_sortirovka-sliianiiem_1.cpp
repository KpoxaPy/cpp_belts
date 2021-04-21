#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    typename RandomIt::difference_type size = range_end - range_begin;
    if (size <= 1) {
        return;
    }
    vector<typename RandomIt::value_type> e(range_begin, range_end);
    RandomIt p = begin(e) + e.size() / 2;
    MergeSort(begin(e), p);
    MergeSort(p, end(e));
    merge(begin(e), p, p, end(e), range_begin);
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

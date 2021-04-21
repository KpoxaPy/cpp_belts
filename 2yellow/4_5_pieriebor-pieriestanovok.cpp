#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

void PrintV(const vector<int>& v) {
    for (const auto& x : v) {
        cout << x << ' ';
    }
    cout << endl;
}

int main() {
    int n;
    cin >> n;

    vector<int> v(n);
    iota(rbegin(v), rend(v), 1);
    PrintV(v);
    while (!is_sorted(begin(v), end(v))) {
        prev_permutation(begin(v), end(v));
        PrintV(v);
    }

    return 0;
}

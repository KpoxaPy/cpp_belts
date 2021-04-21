#include <iostream>
#include <vector>
#include <list>

using namespace std;

const size_t MAX_NUM = 100000;

int main() {
    int n;
    cin >> n;

    list<int> l;
    vector<list<int>::iterator> v(MAX_NUM + 1, l.end());
    while (n-- > 0) {
        int num, pred;
        cin >> num >> pred;
        v[num] = l.insert(v[pred], num);
    }

    for (int x : l) {
        cout << x << '\n';
    }

    return 0;
}
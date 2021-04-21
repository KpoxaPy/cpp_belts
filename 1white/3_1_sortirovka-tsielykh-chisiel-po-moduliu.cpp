#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int n;
    cin >> n;
    vector<int> nums(n);
    for (int& num : nums) {
        cin >> num;
    }
    sort(begin(nums), end(nums), [](const int& a, const int& b) {
        return abs(a) < abs(b);
    });
    for (const int& num : nums) {
        cout << num << " ";
    }
    cout << endl;
    return 0;
}
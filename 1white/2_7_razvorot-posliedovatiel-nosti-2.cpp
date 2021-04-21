#include <vector>
#include <algorithm>
using namespace std;

vector<int> Reversed(const vector<int>& nums) {
    vector<int> v = nums;
    reverse(begin(v), end(v));
    return v;
}
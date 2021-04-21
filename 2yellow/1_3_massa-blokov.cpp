#include <iostream>
using namespace std;

int main() {
    uint64_t n = 0, r = 0;
    cin >> n >> r;
    uint64_t sum = 0;
    for (int i = 0; i < n; ++i) {
        uint64_t w = 0, h = 0, d = 0;
        cin >> w >> h >> d;
        sum += w*h*d;
    }
    cout << sum*r << endl;
    return 0;
}
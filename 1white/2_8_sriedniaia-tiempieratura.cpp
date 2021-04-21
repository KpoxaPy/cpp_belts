#include <iostream>
#include <vector>
using namespace std;

int main()
{
    int n;
    int sum = 0;
    cin >> n;
    vector<int> t(n);
    for (int i = 0; i < n; i++) {
        cin >> t[i];
        sum += t[i];
    }

    int avg = sum / n;
    int more_avg_num = 0;
    for (int d : t) {
        if (d > avg) {
            more_avg_num++;
        }
    }
    cout << more_avg_num << endl;
    for (int i = 0; i < n; i++) {
        if (t[i] > avg) {
            cout << i << " ";
        }
    }

    return 0;
}
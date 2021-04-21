#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

int main() {
    ifstream input("input.txt");
    string line;
    double x;
    cout << fixed << setprecision(3);
    while (input >> x) {
        cout << x << endl;
    }
    return 0;
}
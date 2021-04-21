#include <iostream>
#include <map>

using namespace std;

map<char, size_t> BuildCharCounters(const string& word) {
    map<char, size_t> result;
    for (char c : word) {
        result[c]++;
    }
    return result;
}

int main()
{
    size_t q;    
    cin >> q;

    while (q > 0) {
        q--;
        string a, b;
        cin >> a >> b;
        if (BuildCharCounters(a) == BuildCharCounters(b)) {
            cout << "YES" << endl;
        } else {
            cout << "NO" << endl;
        }
    }
}
#include <map>
#include <iostream>
#include <stdexcept>

using namespace std;

template <class K, class V>
V& GetRefStrict(map<K, V>& m, K key) {
    if (auto it = m.find(key); it != end(m)) {
        return it->second;
    }
    throw runtime_error("");
}

int main() {
    map<int, string> m = {{0, "value"}};
    string& item = GetRefStrict(m, 0);
    item = "newvalue";
    cout << m[0] << endl; // выведет newvalue
    return 0;
}
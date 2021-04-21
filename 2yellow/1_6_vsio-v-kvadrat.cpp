#include <vector>
#include <map>
#include <iostream>

using namespace std;

template <class T> vector<T> Sqr(const vector<T>&);
template <class K, class V> map<K, V> Sqr(const map<K, V>&);
template <class E1, class E2> pair<E1, E2> Sqr(const pair<E1, E2>&);

template <class T>
T Sqr(const T& x) {
    return x*x;
}

template <class T>
vector<T> Sqr(const vector<T>& v) {
    vector<T> r(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        r[i] = Sqr(v[i]);
    }
    return r;
}

template <class K, class V>
map<K, V> Sqr(const map<K, V>& m) {
    map<K, V> r;
    for (const auto& [k, v] : m) {
        r[k] = Sqr(v);
    }
    return r;
}

template <class E1, class E2>
pair<E1, E2> Sqr(const pair<E1, E2>& p) {
    pair<E1, E2> r;
    r.first = Sqr(p.first);
    r.second = Sqr(p.second);
    return r;
}

// int main() {
//     // Пример вызова функции
//     vector<int> v = {1, 2, 3};
//     cout << "vector:";
//     for (int x : Sqr(v)) {
//     cout << ' ' << x;
//     }
//     cout << endl;

//     map<int, pair<int, int>> map_of_pairs = {
//     {4, {2, 2}},
//     {7, {4, 3}}
//     };
//     cout << "map of pairs:" << endl;
//     for (const auto& x : Sqr(map_of_pairs)) {
//     cout << x.first << ' ' << x.second.first << ' ' << x.second.second << endl;
//     }
// }
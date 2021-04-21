#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>

using namespace std;

void tolower(string& s) {
    for (auto& c : s) {
        c = tolower(c);
    }
}

string tolower(const string& s) {
    string result = s;
    tolower(result);
    return result;
}

int main() {
    int n;
    cin >> n;
    vector<string> words(n);
    for (auto& word : words) {
        cin >> word;
    }
    sort(begin(words), end(words), [](const string& l, const string& r) {
        return tolower(l) < tolower(r);
    });
    for (const auto& word : words) {
        cout << word << " ";
    }
    cout << endl;
    return 0;
}
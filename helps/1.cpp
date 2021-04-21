#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_map>

using namespace std;
/*
bool compare(const string& s1, const string& s2) {
    if (s1.size() != s2.size()) {
        return false;
    } else {
        vector<char> secondCopy = {s2.begin(), s2.end()};
        map<char, char> exchangeRate;
        for (int i = 0; i < s1.size(); ++i) {
            if (exchangeRate.count(s1[i]) == 0) {
                exchangeRate[s1[i]] = s2[i];
            } else {
                secondCopy[i] = exchangeRate[s1[i]];
            }
        }
        string second = {secondCopy.begin(), secondCopy.end()};
        if (second != s2) {
            return  false;
        } else {
            return true;
        }
    }
}*/

bool compareREM(const string& s1, const string& s2) {
    if (s1.size() != s2.size()) {
        return false;
    } else if (s1 == s2) {
        return true;
    }
        else {
        unordered_map<char, char> exchangeRate;
        unordered_map<char, char> exchangeRateV;
        for (int i = 0; i < s1.size(); ++i) {
            if (exchangeRate.count(s1[i]) == 0 && exchangeRateV.count(s2[i]) == 0) {
                exchangeRate[s1[i]] = s2[i];
                exchangeRateV[s2[i]] = s1[i];
            } else {
                if (exchangeRate[s1[i]] != s2[i] || exchangeRateV[s2[i]] != s1[i]) {
                    return false;
                }
            }
        }
        return true;
    }
}

int main() {
    std::cin.tie(nullptr);
    string first, second;
    cin >> first >> second;
    
    if (compareREM(first, second)) {
        cout << "YES" << endl;
    } else {
        cout << "NO" << endl;
    }
    return 0;
}
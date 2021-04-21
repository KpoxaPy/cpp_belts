#include <string>
#include <vector>
using namespace std;

bool IsPalindrom(string w) {
    for (int i = 0; i < w.length() / 2; i++) {
        if (w[i] != w[w.length() - 1 - i])
            return false;
    }
    return true;
}

vector<string> PalindromFilter(vector<string> words, int minLength) {
    vector<string> result;
    for (string w : words) {
        if (w.size() >= minLength) {
            if (IsPalindrom(w)) {
                result.push_back(w);
            }
        }
    }
    return result;
}


// #include <iostream>

// string PrintWords(vector<string> words) {
//     string result;
//     bool isNotFirst = false;
//     for (string w : words) {
//         if (!isNotFirst) {
//             isNotFirst = true;
//         } else {
//             result += ", ";
//         }
//         result += w;
//     }
//     return "{" + result + "}";
// }

// void TestIsPalindromFilter(vector<string> words, int minLength, vector<string> answer) {
//     vector<string> result = PalindromFilter(words, minLength);
//     if (result == answer) {
//         cout << "OK   PalindromFilter(" << PrintWords(words) << ", " << minLength << ") returns " << PrintWords(result) << endl;
//     } else {
//         cout << "FAIL PalindromFilter(" << PrintWords(words) << ", " << minLength << ") returns " << PrintWords(result) << ", but should " << PrintWords(answer) << endl;
//     }
// }

// int main() {
//     TestIsPalindromFilter({"abacaba", "aba"}, 5, {"abacaba"});
//     TestIsPalindromFilter({"abacaba", "aba"}, 2, {"abacaba", "aba"});
//     TestIsPalindromFilter({"weew", "bro", "code"}, 4, {"weew"});
//     return 0;
// }
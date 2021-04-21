#include <string>
using namespace std;

bool IsPalindrom(string w) {
    for (int i = 0; i < w.length() / 2; i++) {
        if (w[i] != w[w.length() - 1 - i])
            return false;
    }
    return true;
}

// #include <iostream>

// void TestIsPalindrom(string w, bool answer) {
//     bool result = IsPalindrom(w);
//     if (result == answer) {
//         cout << "OK   IsPalindrom(" << w << ") returns " << (result ? "TRUE" : "FALSE") << endl;
//     } else {
//         cout << "FAIL IsPalindrom(" << w << ") returns " << (result ? "TRUE" : "FALSE") << ", but should " << (answer ? "TRUE" : "FALSE") << endl;
//     }
// }

// int main() {
//     TestIsPalindrom("madam", true);
//     TestIsPalindrom("maam", true);
//     TestIsPalindrom("gentleman", false);
//     TestIsPalindrom("X", true);
//     return 0;
// }
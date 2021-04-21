#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

class ReversibleString
{
public:
    ReversibleString() = default;

    ReversibleString(const string& s) {
        s_ = s;
    }

    const string& ToString() const {
        return s_;
    }

    void Reverse() {
        reverse(begin(s_), end(s_));
    }

private:
    string s_;
};

// int main() {
//   ReversibleString s("live");
//   s.Reverse();
//   cout << s.ToString() << endl;
  
//   s.Reverse();
//   const ReversibleString& s_ref = s;
//   string tmp = s_ref.ToString();
//   cout << tmp << endl;
  
//   ReversibleString empty;
//   cout << '"' << empty.ToString() << '"' << endl;
  
//   return 0;
// }

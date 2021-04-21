#include <iostream>
using namespace std;

int main()
{
  string s;
  cin >> s;
  bool first_meet = false;
  int idx = 0;
  for (int i = 0; i < s.size(); ++i) {
    if (s[i] == 'f') {
        if (!first_meet) {
            first_meet = true;
        } else {
            idx = i;
            break;
        }
    }
  }
  if (idx > 0) {
    cout << idx;
  } else if (first_meet) {
    cout << -1;
  } else {
    cout << -2;
  }
  return 0;
}

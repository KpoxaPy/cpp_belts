#include <iostream>

using namespace std;

int main() {
  int64_t a, b;
  cin >> a >> b;
  int64_t res;
  if (!__builtin_add_overflow(a, b, &res)) {
    cout << res << endl;
  } else {
    cout << "Overflow!" << endl;
  }
  return 0;
}
#include <iostream>
#include <vector>
using namespace std;

int main()
{
  int n;
  cin >> n;

  vector<bool> bits;
  while (n > 0) {
    bits.push_back(n % 2);
    n /= 2;
  }
  for(int i = bits.size() - 1; i >= 0; --i) {
    if (bits[i]) {
      cout << 1;
    } else {
      cout << 0;
    }
  }
}

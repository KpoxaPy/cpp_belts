#include <iostream>
using namespace std;

int main()
{
  int m, n;
  cin >> m >> n;
  int nod = 1;
  while (true) {
    if (m == 1) {
        break;
    }
    if (n == 1) {
        break;
    }
    if (n == 0 || n == m) {
        nod *= m;
        break;
    }
    if (m == 0) {
        nod *= n;
        break;
    }
    if (m % 2 == 0 && n % 2 == 0) {
        m /= 2;
        n /= 2;
        nod *= 2;
    } else if (m % 2 == 0) {
        m /= 2;
    } else if (n % 2 == 0) {
        n /= 2;
    } else {
        if (m > n) {
            m = (m - n) / 2;
        } else {
            int d = (n - m) / 2;
            n = m;
            m = d;
        }
    }
  }
  cout << nod;
  return 0;
}

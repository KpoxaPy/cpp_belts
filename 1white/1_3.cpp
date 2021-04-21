#include <iostream>
#include <cmath>
using namespace std;

int main()
{
  double a;
  double b;
  double c;
  cin >> a >> b >> c;
  if (a != 0) {
    double d = b * b - 4 * a *c;
    if (d == 0) {
        cout << -b / (2 * a);
    } else if (d > 0) {
        double sqrtd = sqrt(d);
        cout << (-b + sqrtd) / (2 * a) << " " << (-b - sqrtd) / (2 * a);
    }
  } else if (b != 0) {
    cout << -c / b;
  }
}

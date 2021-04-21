#include <iostream>
#include <string>
using namespace std;

int main()
{
  string a;
  string b;
  string c;
  cin >> a >> b >> c;
  string min = a;
  if (b < min) {
      min = b;
  }
  if (c < min) {
      min = c;
  }
  cout << min;
}

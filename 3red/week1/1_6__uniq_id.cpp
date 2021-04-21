#include <string>
#include <vector>
using namespace std;

#define UNIQ_ID__(v) __uniq_ ## v
#define UNIQ_ID_(v) UNIQ_ID__(v)
#define UNIQ_ID UNIQ_ID_(__LINE__)

int main() {
  int UNIQ_ID = 0;
  string UNIQ_ID = "hello";
  vector<string> UNIQ_ID = {"hello", "world"};
  vector<int> UNIQ_ID = {1, 2, 3, 4};
}
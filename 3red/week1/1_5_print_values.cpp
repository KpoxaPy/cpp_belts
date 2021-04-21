#include "test_runner.h"

#include <ostream>
using namespace std;

#define PRINT_VALUES(out, x, y) do { \
  out << (x) << endl;                \
  out << (y) << endl;                \
} while (false)                      \

int main() {
  TestRunner tr;
  tr.RunTest([] {
    ostringstream output;
    PRINT_VALUES(output, 5, "red belt");
    ASSERT_EQUAL(output.str(), "5\nred belt\n");
  }, "PRINT_VALUES usage example");

  if (true) PRINT_VALUES(cout, 1, 2); else PRINT_VALUES(cout, 3, 4);
}

#include <iostream>
#include <fstream>

#include "sprav.h"
#include "spravio.h"
#include "tests.h"

using namespace std;

int main() {
  TestAll();

  SpravPtr sprav = make_shared<Sprav>();
  SpravIO sprav_io(sprav, cout, Format::JSON_PRETTY);
  sprav_io.Process(cin);

  return 0;
}
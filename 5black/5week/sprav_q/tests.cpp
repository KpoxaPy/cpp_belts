#include "tests.h"
#include "test_runner.h"

#include "reader_tests.h"
#include "spravio_tests.h"
#include "sprav_tests.h"

using namespace std;


void TestAll() {
  TestRunner tr;
  TestInputReader(tr);
  TestSpravIO(tr);
  TestSprav(tr);
}
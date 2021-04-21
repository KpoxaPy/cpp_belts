#include "database.h"
#include "command.h"
#include "tests.h"

#include <iostream>

using namespace std;

int main() {
  TestAll();

  Database db;
  for (string line; getline(cin, line);){
    ParseCommand(line)->Evaluate(db);
  }

  return 0;
}

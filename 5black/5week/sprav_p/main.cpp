#include <iostream>
#include <fstream>
#include <string_view>

#include "sprav.h"
#include "spravio.h"
#include "tests.h"

using namespace std;

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    cerr << "Usage: transport_catalog_part_o [make_base|process_requests]\n";
    return 5;
  }

  const string_view mode(argv[1]);

  SpravPtr sprav = make_shared<Sprav>();
  if (mode == "make_base") {
    SpravIO(sprav, SpravIO::Mode::MAKE_BASE, cout).Process(cin);
  } else if (mode == "process_requests") {
    SpravIO(sprav, SpravIO::Mode::PROCESS_REQUESTS, cout).Process(cin);
  }

  return 0;
}

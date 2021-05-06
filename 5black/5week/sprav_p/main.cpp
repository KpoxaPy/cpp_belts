#include <iostream>
#include <fstream>
#include <string_view>

#include "sprav.h"
#include "spravio.h"
#include "tests.h"

using namespace std;

void Do(std::string_view mode) {
  LOG_DURATION("Do");
  SpravPtr sprav = make_shared<Sprav>();
  if (mode == "make_base") {
    SpravIO(sprav, SpravIO::Mode::MAKE_BASE, cout).Process(cin);
  } else if (mode == "process_requests") {
    SpravIO(sprav, SpravIO::Mode::PROCESS_REQUESTS, cout).Process(cin);
  }
}

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    cerr << "Usage: transport_catalog_part_o [make_base|process_requests]\n";
    return 5;
  }

  Do(argv[1]);

  return 0;
}

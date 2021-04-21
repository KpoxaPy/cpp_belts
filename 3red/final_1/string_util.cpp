#include "string_util.h"
#include "parse.h"

#include <sstream>
#include <iterator>

using namespace std;

list<string_view> SplitIntoWords(string_view line) {
  list<string_view> r;
  while (!line.empty()) {
    while (!line.empty() && isspace(line.front())) {
      line.remove_prefix(1);
    }
    if (line.empty()) {
      break;
    }
    size_t i = 0;
    for (; i != line.size() && !isspace(line[i]); ++i) {
    }
    r.push_back(line.substr(0, i));
    line.remove_prefix(i);
  }
  return r;
}
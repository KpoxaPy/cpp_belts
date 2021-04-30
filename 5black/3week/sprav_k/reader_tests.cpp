#include "reader_tests.h"
#include "reader.h"
#include <sstream>

using namespace std;

namespace InputReaderTests {

void Helper(InputReader& reader, const string& input, vector<string> expected) {
  vector<string> result;
  istringstream is(input);

  reader.SetCallback([&result](string line) {
    result.emplace_back(move(line));
  });
  reader.Process(is);
  ASSERT_EQUAL(result, expected);
}

void WithCounter() {
  InputReader reader;
  reader.SetUseCounter(true);
  Helper(reader,
    "",
    {});
  Helper(reader,
    "1\n"
    "a\n",
    {"a"});
  Helper(reader,
    "1\n"
    "a",
    {"a"});
  Helper(reader,
    "2\n"
    "a\n"
    "b",
    {"a", "b"});
  Helper(reader,
    "2\n"
    "a\n"
    "b\n"
    "c\n"
    "d\n",
    {"a", "b"});
}

void WOCounter() {
  InputReader reader;
  reader.SetUseCounter(false);
  Helper(reader,
    "",
    {});
  Helper(reader,
    "a",
    {"a"});
  Helper(reader,
    "a\n"
    "b",
    {"a", "b"});
  Helper(reader,
    "a\n"
    "b\n",
    {"a", "b"});
}

}

void TestInputReader(TestRunner& tr) {
  RUN_TEST(tr, InputReaderTests::WithCounter);
  RUN_TEST(tr, InputReaderTests::WOCounter);
}
#include "test_runner.h"

#include <algorithm>
#include <numeric>
#include <cstring>
using namespace std;

template <typename T>
void Swap(T* first, T* second) {
  T temp = *second;
  *second = *first;
  *first = temp;
}

template <typename T>
void SortPointers(vector<T*>& pointers) {
  sort(begin(pointers), end(pointers), [](T* lhs, T* rhs) {
    return *lhs < *rhs;
  });
}

template <typename T>
void ReversedCopy(T* source, size_t count, T* destination) {
  if (source <= destination && destination < source + count) {
    reverse(destination, source + count);
    size_t c = destination - source;
    destination += count - 1;
    for (; c > 0; --c) {
      *destination-- = *source++;
    }
    return;
  }

  size_t c = count;
  if (destination <= source && source < destination + count) {
    reverse(source, destination + count);  
    c = source - destination;
  }

  source += count - 1;
  for (; c > 0; --c) {
    *destination++ = *source--;
  }
}

void TestSwap() {
  int a = 1;
  int b = 2;
  Swap(&a, &b);
  ASSERT_EQUAL(a, 2);
  ASSERT_EQUAL(b, 1);

  string h = "world";
  string w = "hello";
  Swap(&h, &w);
  ASSERT_EQUAL(h, "hello");
  ASSERT_EQUAL(w, "world");
}

void TestSortPointers() {
  int one = 1;
  int two = 2;
  int three = 3;

  vector<int*> pointers;
  pointers.push_back(&two);
  pointers.push_back(&three);
  pointers.push_back(&one);

  SortPointers(pointers);

  ASSERT_EQUAL(pointers.size(), 3u);
  ASSERT_EQUAL(*pointers[0], 1);
  ASSERT_EQUAL(*pointers[1], 2);
  ASSERT_EQUAL(*pointers[2], 3);
}

void TestReverseCopy() {
  const size_t count = 7;

  int* source = new int[count];
  int* dest = new int[count];

  for (size_t i = 0; i < count; ++i) {
    source[i] = i + 1;
  }
  ReversedCopy(source, count, dest);
  const vector<int> expected1 = {7, 6, 5, 4, 3, 2, 1};
  ASSERT_EQUAL(vector<int>(dest, dest + count), expected1);

  // Области памяти могут перекрываться
  ReversedCopy(source, count - 1, source + 1);
  const vector<int> expected2 = {1, 6, 5, 4, 3, 2, 1};
  ASSERT_EQUAL(vector<int>(source, source + count), expected2);

  for (size_t i = 0; i < count; ++i) {
    source[i] = i + 1;
  }
  ReversedCopy(source + 1, count - 1, source);
  const vector<int> expected3 = {7, 6, 5, 4, 3, 2, 7};
  ASSERT_EQUAL(vector<int>(source, source + count), expected3);

  delete[] dest;
  delete[] source;
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSwap);
  RUN_TEST(tr, TestSortPointers);
  RUN_TEST(tr, TestReverseCopy);
  return 0;
}

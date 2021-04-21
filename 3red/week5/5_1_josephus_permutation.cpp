#include "test_runner.h"
#include "profile.h"

#include <cstdint>
#include <iterator>
#include <numeric>
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <iostream>

using namespace std;

template <typename Container, typename ForwardIt>
ForwardIt LoopIterator(Container& container, ForwardIt pos) {
  return pos == container.end() ? container.begin() : pos;
}

template <typename RandomIt>
void MakeJosephusPermutation(RandomIt first, RandomIt last,
                             uint32_t step_size) {
  list<typename RandomIt::value_type> pool;
  { LOG_DURATION("fill");
    for (auto it = first; it != last; ++it) {
      pool.push_back(move(*it));
    }
  }
  { LOG_DURATION("calc");
    auto cur_pos = pool.begin();
    while (!pool.empty()) {
      *(first++) = move(*cur_pos);
      if (pool.size() == 1) {
        break;
      }
      const auto next_pos = LoopIterator(pool, next(cur_pos));
      pool.erase(cur_pos);
      cur_pos = next_pos;
      for (uint32_t step_index = 1; step_index < step_size; ++step_index) {
        cur_pos = LoopIterator(pool, next(cur_pos));
      }
    }
  }
}


// template <typename RandomIt>
// void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {
//   if (step_size == 1) {
//     return;
//   }

//   map<size_t, typename RandomIt::value_type> pool;
//   {
//     size_t cur_pos = 0;
//     auto it = first;
//     for (; it != last; ++it, ++cur_pos) {
//       pool[cur_pos] = move(*it);
//     }
//   }

//   auto it = begin(pool);
//   while (!pool.empty()) {
//     *(first++) = move(it->second);
//     it = pool.erase(it);
//     if (pool.empty()) {
//       break;
//     }
//     if (it == end(pool)) {
//       it = begin(pool);
//     }
//     for (int i = (step_size - 1) % pool.size(); i > 0; --i) {
//       advance(it, 1);
//       if (it == end(pool)) {
//         it = begin(pool);
//       }
//     }
//   }
// }

// template <typename RandomIt>
// void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {
//   deque<typename RandomIt::value_type> pool(make_move_iterator(first), make_move_iterator(last));
//   size_t cur_pos = 0;
//   while (!pool.empty()) {
//     *(first++) = move(pool[cur_pos]);
//     pool.erase(pool.begin() + cur_pos);
//     if (pool.empty()) {
//       break;
//     }
//     cur_pos = (cur_pos + step_size - 1) % pool.size();
//   }
// }

// template <typename RandomIt>
// void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {
//   vector<typename RandomIt::value_type> pool(first, last);
//   size_t cur_pos = 0;
//   while (!pool.empty()) {
//     *(first++) = pool[cur_pos];
//     pool.erase(pool.begin() + cur_pos);
//     if (pool.empty()) {
//       break;
//     }
//     cur_pos = (cur_pos + step_size - 1) % pool.size();
//   }
// }

vector<int> MakeTestVector() {
  vector<int> numbers(10);
  iota(begin(numbers), end(numbers), 0);
  return numbers;
}

void TestIntVector() {
  const vector<int> numbers = MakeTestVector();
  {
    vector<int> numbers_copy = numbers;
    MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 1);
    ASSERT_EQUAL(numbers_copy, numbers);
  }
  {
    vector<int> numbers_copy = numbers;
    MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 3);
    ASSERT_EQUAL(numbers_copy, vector<int>({0, 3, 6, 9, 4, 8, 5, 2, 7, 1}));
  }
}

// Это специальный тип, который поможет вам убедиться, что ваша реализация
// функции MakeJosephusPermutation не выполняет копирование объектов.
// Сейчас вы, возможно, не понимаете как он устроен, однако мы расскажем,
// почему он устроен именно так, далее в блоке про move-семантику —
// в видео «Некопируемые типы»

struct NoncopyableInt {
  int value;

  NoncopyableInt(int value) : value(value) {}

  NoncopyableInt(const NoncopyableInt&) = delete;
  NoncopyableInt& operator=(const NoncopyableInt&) = delete;

  NoncopyableInt(NoncopyableInt&&) = default;
  NoncopyableInt& operator=(NoncopyableInt&&) = default;
};

bool operator == (const NoncopyableInt& lhs, const NoncopyableInt& rhs) {
  return lhs.value == rhs.value;
}

ostream& operator << (ostream& os, const NoncopyableInt& v) {
  return os << v.value;
}

// void TestAvoidsCopying() {
//   vector<NoncopyableInt> numbers;
//   numbers.push_back({1});
//   numbers.push_back({2});
//   numbers.push_back({3});
//   numbers.push_back({4});
//   numbers.push_back({5});

//   MakeJosephusPermutation(begin(numbers), end(numbers), 2);

//   vector<NoncopyableInt> expected;
//   expected.push_back({1});
//   expected.push_back({3});
//   expected.push_back({5});
//   expected.push_back({4});
//   expected.push_back({2});

//   ASSERT_EQUAL(numbers, expected);
// }

struct LoggingInt {
  int value;

  LoggingInt() : value(0) {}

  LoggingInt(int value) : value(value) {}

  LoggingInt(const LoggingInt& v) {
    value = v.value;
    copies++;
  }

  LoggingInt& operator=(const LoggingInt& v) {
    value = v.value;
    copies++;
    return *this;
  }

  LoggingInt(LoggingInt&& v) {
    value = v.value;
    moves++;
  }

  LoggingInt& operator=(LoggingInt&& v) {
    value = v.value;
    moves++;
    return *this;
  }

  inline static void ResetStat() {
    copies = 0;
    moves = 0;
  }

  inline static size_t copies = 0;
  inline static size_t moves = 0;
};

void TestLarge() {
  LOG_DURATION("TestLarge");
  vector<LoggingInt> numbers(100000);
  iota(begin(numbers), end(numbers), 0);

  size_t copies = 0;
  size_t moves = 0;
  for (int i = 1; i <= 100; i += 10) {
    vector<LoggingInt> numbers_copy = numbers;
    LoggingInt::ResetStat();
    MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), i);
    copies += LoggingInt::copies;
    moves += LoggingInt::moves;
  }

  cout << "Copies = " << copies << endl;
  cout << "Moves = " << moves << endl;
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestIntVector);
  // RUN_TEST(tr, TestAvoidsCopying);
  RUN_TEST(tr, TestLarge);
  return 0;
}

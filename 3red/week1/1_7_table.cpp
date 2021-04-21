#include "test_runner.h"

using namespace std;

template <class T>
class Table {
public:
  Table(size_t rows, size_t cols)
  {
    Resize(rows, cols);
  }

  void Resize(size_t rows, size_t cols) {
    rows_ = rows;
    cols_ = cols;
    if (rows_ == 0 || cols_ == 0) {
      rows_ = 0;
      cols_ = 0;
    }

    table_.resize(rows_, vector<T>(cols_, T()));
    for (auto& v : table_) {
      v.resize(cols, T());
    }
  }

  vector<T>& operator[](size_t i) {
    return table_[i];
  }

  const vector<T>& operator[](size_t i) const {
    return table_[i];
  }

  pair<size_t, size_t> Size() const {
    return {rows_, cols_};
  }

private:
  size_t rows_, cols_;
  vector<vector<T>> table_;
};

void TestTable() {
  Table<int> t(1, 1);
  ASSERT_EQUAL(t.Size().first, 1u);
  ASSERT_EQUAL(t.Size().second, 1u);
  t[0][0] = 42;
  ASSERT_EQUAL(t[0][0], 42);
  t.Resize(3, 4);
  ASSERT_EQUAL(t.Size().first, 3u);
  ASSERT_EQUAL(t.Size().second, 4u);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestTable);
  return 0;
}

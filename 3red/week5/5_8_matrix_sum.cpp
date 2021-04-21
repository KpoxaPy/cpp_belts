#include "test_runner.h"
#include "profile.h"
#include <vector>
#include <numeric>
#include <future>

using namespace std;

template <typename Iterator>
class Paginator {
public:
  class Page {
  public:
    Page(Iterator first, Iterator last)
      : first_(first), last_(last)
    {}

    Iterator begin() const {
      return first_;
    }

    Iterator end() const {
      return last_;
    }

    size_t size() const {
      return last_ - first_;
    }

  private:
    Iterator first_, last_;
  };

public:
  Paginator(Iterator first, Iterator last, size_t page_size)
    : page_size_(page_size)
  {
    size_t size = last - first;

    while (first != last) {
      Iterator next = first;
      if (size >= page_size) {
        next = first + page_size;
        size -= page_size;
      } else {
        next = first + size;
        size = 0;
      }
      pages_.emplace_back(first, next);
      first = next;
    }
  }

  auto begin() const {
    return pages_.begin();
  }

  auto end() const {
    return pages_.end();
  }

  size_t size() const {
    return pages_.size();
  }

private:
  std::vector<Page> pages_;
  size_t page_size_;
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
  return Paginator(begin(c), end(c), page_size);
}

template <typename InputIt>
int64_t CalculateMatrixSumST(InputIt first, InputIt last) {
  return accumulate(first, last, static_cast<int64_t>(0),
    [](int64_t& sum, const auto& v){
      return sum + accumulate(begin(v), end(v), static_cast<int64_t>(0));
    });
}

int64_t CalculateMatrixSumMT(const vector<vector<int>>& matrix) {
  const size_t threads = 4;

  vector<future<int64_t>> futs;
  for (auto p : Paginate(matrix, matrix.size() / threads + 1)) {
    futs.push_back(async([p]{ return CalculateMatrixSumST(begin(p), end(p)); }));
  }

  return accumulate(begin(futs), end(futs), static_cast<int64_t>(0), [](auto& s, auto& f) {
    return s += f.get();
  });
}

int64_t CalculateMatrixSum(const vector<vector<int>>& matrix) {
  return CalculateMatrixSumMT(matrix);
}

vector<vector<int>> MakeBigMatrix(size_t n = 9000) {
  vector<vector<int>> r(n);

  for (size_t i = 0; i < n; ++i) {
    r[i].resize(n);
    for (size_t j = 0; j < n; ++j) {
      r[i][j] = i ^ j;
    }
  }

  return r;
}

void TestCalculateMatrixSum() {
  const vector<vector<int>> matrix = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12},
    {13, 14, 15, 16}
  };
  ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
}

// void TestBigMatrixSum() {
//   const vector<vector<int>> m = MakeBigMatrix();

//   AVG_DURATION("Single thread big sum", 10, {
//     CalculateMatrixSum(m);
//   });
// }

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestCalculateMatrixSum);
  // RUN_TEST(tr, TestBigMatrixSum);
}

#pragma once

#include <vector>

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

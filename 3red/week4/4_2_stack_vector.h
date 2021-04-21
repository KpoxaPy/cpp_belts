#pragma once

#include <stdexcept>
using namespace std;

template <typename T, size_t N>
class StackVector {
public:
  explicit StackVector(size_t a_size = 0)
    : size_(a_size)
  {
    if (size_ > capacity_) {
      throw invalid_argument("");
    }
  }

  T& operator[](size_t index) {
    return data_[index];
  }
  const T& operator[](size_t index) const {
    return data_[index];
  }

  T* begin() {
    return data_;
  }

  T* end() {
    return data_ + size_;
  }

  const T* begin() const {
    return data_;
  }

  const T* end() const {
    return data_ + size_;
  }

  size_t Size() const {
    return size_;
  }

  size_t Capacity() const {
    return capacity_;
  }

  void PushBack(const T& value) {
    if (size_ == capacity_) {
      throw overflow_error("");
    }
    data_[size_++] = value;
  }

  T PopBack() {
    if (size_ == 0) {
      throw underflow_error("");
    }
    return data_[--size_];
  }

private:
  static const size_t capacity_ = N;

  T data_[N];
  size_t size_;
};

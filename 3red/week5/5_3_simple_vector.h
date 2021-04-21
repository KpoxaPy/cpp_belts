#pragma once

#include <cstdlib>
#include <algorithm>

using namespace std;

template <typename T>
class SimpleVector {
public:
  SimpleVector() {}

  explicit SimpleVector(size_t size)
    : size_(size)
  {
    Reserve(size);
  }

  ~SimpleVector() {
    if (data_ != nullptr) {
      delete[] data_;
    }
  }

  SimpleVector& operator=(const SimpleVector& other) {
    if (data_ != nullptr) {
      delete[] data_;
    }
    data_ = new T[other.capacity_];
    capacity_ = other.capacity_;
    size_ = other.size_;
    copy(other.data_, other.data_ + other.size_, data_);
    return *this;
  }

  T& operator[](size_t index) {
    return data_[index];
  }

  T* begin() {
    return data_;
  }

  T* end() {
    return &data_[size_];
  }

  size_t Size() const {
    return size_;
  }

  size_t Capacity() const {
    return capacity_;
  }

  void PushBack(const T& value) {
    if (capacity_ == 0) {
      Reserve(1);
    } else if (capacity_ == size_) {
      Reserve(capacity_ * 2);
    }
    data_[size_++] = value;
  }

  void Reserve(size_t size) {
    T* new_data = new T[size];
    if (data_ != nullptr) {
      for (size_t i = 0; i < size_; ++i) {
        new_data[i] = data_[i];
      }
      delete[] data_;
    }
    data_ = new_data;
    capacity_ = size;
  }

private:
  T* data_ = nullptr;
  size_t capacity_ = 0;
  size_t size_ = 0;
};

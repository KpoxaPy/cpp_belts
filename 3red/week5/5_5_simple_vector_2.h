#include <cstdint>
#include <utility>
// #include <algorithm>

using namespace std;

// Реализуйте SimpleVector в этом файле
// и отправьте его на проверку

template <typename T>
class SimpleVector {
public:
  SimpleVector() = default;

  explicit SimpleVector(size_t size)
    : size_(size)
  {
    Reserve(size);
  }

  // SimpleVector(const SimpleVector& rhs) {
  //   if (data_ != nullptr) {
  //     delete[] data_;
  //   }
  //   data_ = new T[rhs.capacity_];
  //   capacity_ = rhs.capacity_;
  //   size_ = rhs.size_;
  //   copy(rhs.begin(), rhs.end(), begin());
  // }

  ~SimpleVector() {
    if (data_ != nullptr) {
      delete[] data_;
    }
  }

  // SimpleVector& operator=(const SimpleVector &rhs) {
  //   if (rhs.size_ <= capacity_) {
  //     copy(rhs.begin(), rhs.end(), begin());
  //     size_ = rhs.size_;
  //   } else {
  //     SimpleVector<T> tmp(rhs);
  //     swap(tmp.data_, data_);
  //     swap(tmp.size_, size_);
  //     swap(tmp.capacity_, capacity_);
  //   }
  //   return *this;
  // }

  T& operator[](size_t index) {
    return data_[index];
  }

  T* begin() {
    return data_;
  }

  T* end() {
    return &data_[size_];
  }

  const T* begin() const {
    return data_;
  }

  const T* end() const {
    return &data_[size_];
  }

  size_t Size() const {
    return size_;
  }

  size_t Capacity() const {
    return capacity_;
  }

  void PushBack(const T& value) {
    PushBack(T(value));
  }

  void PushBack(T&& value) {
    if (capacity_ == 0) {
      Reserve(1);
    } else if (capacity_ == size_) {
      Reserve(capacity_ * 2);
    }
    data_[size_++] = move(value);
  }

  void Reserve(size_t size) {
    T* new_data = new T[size];
    if (data_ != nullptr) {
      for (size_t i = 0; i < size_; ++i) {
        new_data[i] = move(data_[i]);
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

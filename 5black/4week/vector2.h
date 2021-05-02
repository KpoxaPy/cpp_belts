#include <cstddef>
#include <utility>
#include <memory>
#include <algorithm>

template <typename T>
struct RawMemory {
  T* buf = nullptr;
  size_t capacity = 0;

  RawMemory() = default;
  RawMemory(size_t n) {
    buf = static_cast<T*>(operator new (n*sizeof(T)));
    capacity = n;
  }

  RawMemory(const RawMemory&) = delete;
  RawMemory(RawMemory&& other) {
    Swap(other);
  }

  RawMemory& operator =(const RawMemory&) = delete;
  RawMemory& operator =(RawMemory&& other) {
    Swap(other);
    return *this;
  }

  ~RawMemory() {
    if (buf) {
      operator delete(buf);
    }
  }

  void Swap(RawMemory& other) {
    std::swap(buf, other.buf);
    std::swap(capacity, other.capacity);
  }
};

template <typename T>
class Vector {
public:
  using iterator = T*;
  using const_iterator = const T*;

public:
  Vector() = default;

  Vector(size_t n)
    : data_(n)
  {
    std::uninitialized_value_construct_n(data_.buf, n);
    size_ = n;
  }

  Vector(const Vector& other)
    : data_(other.size_)
  {
    std::uninitialized_copy_n(other.data_.buf, other.size_, data_.buf);
    size_ = other.size_;
  }

  Vector(Vector&& other) {
    Swap(other);
  }

  ~Vector() {
    std::destroy_n(data_.buf, size_);
  }

  Vector& operator=(const Vector& other) {
    if (data_.capacity < other.size_) {
      Vector tmp(other);
      Swap(tmp);
    } else {
      for (size_t i = 0; i < size_ && i < other.size_; ++i) {
        data_.buf[i] = other.data_.buf[i];
      }
      if (size_ > other.size_) {
        std::destroy_n(data_.buf + other.size_, size_ - other.size_);
      } else if (size_ < other.size_) {
        std::uninitialized_copy_n(other.data_.buf + size_, other.size_ - size_, data_.buf + size_);
      }
      size_ = other.size_;
    }

    return *this;
  }

  Vector& operator=(Vector&& other) noexcept {
    Swap(other);
    return *this;
  }

  void Reserve(size_t n) {
    if (n > data_.capacity) {
      RawMemory<T> new_data(n);
      std::uninitialized_move_n(data_.buf, size_, new_data.buf);
      std::destroy_n(data_.buf, size_);
      data_ = std::move(new_data);
    }
  }

  void Resize(size_t n) {
    Reserve(n);
    if (n > size_) {
      std::uninitialized_value_construct_n(data_.buf + size_, n - size_);
    } else if (n < size_) {
      std::destroy_n(data_.buf + n, size_ - n);
    }
    size_ = n;
  }

  void PushBack(const T& elem) {
    if (size_ == data_.capacity) {
      Reserve(size_ == 0 ? 1 : size_ * 2);
    }
    new (data_.buf + size_) T(elem);
    ++size_;
  }

  void PushBack(T&& elem) {
    if (size_ == data_.capacity) {
      Reserve(size_ == 0 ? 1 : size_ * 2);
    }
    new (data_.buf + size_) T(std::move(elem));
    ++size_;
  }

  template <typename ... Args>
  T& EmplaceBack(Args&&... args) {
    if (size_ == data_.capacity) {
      Reserve(size_ == 0 ? 1 : size_ * 2);
    }
    new (data_.buf + size_) T(std::forward<Args>(args)...);
    ++size_;
    return data_.buf[size_ - 1];
  }

  void PopBack() {
    std::destroy_at(data_.buf + size_ - 1);
    --size_;
  }

  size_t Size() const noexcept {
    return size_;
  }

  size_t Capacity() const noexcept {
    return data_.capacity;
  }

  const T& operator[](size_t i) const {
    return data_.buf[i];
  }

  T& operator[](size_t i) {
    return data_.buf[i];
  }

  void Swap(Vector& other) {
    data_.Swap(other.data_);
    std::swap(size_, other.size_);
  }

  iterator begin() noexcept {
    return data_.buf;
  }

  iterator end() noexcept {
    return data_.buf + size_;
  }

  const_iterator begin() const noexcept {
    return data_.buf;
  }

  const_iterator end() const noexcept {
    return data_.buf + size_;
  }

  const_iterator cbegin() const noexcept {
    return data_.buf;
  }

  const_iterator cend() const noexcept {
    return data_.buf + size_;
  }

  // Вставляет элемент перед pos
  // Возвращает итератор на вставленный элемент
  iterator Insert(const_iterator pos, const T& elem) {
    size_t n = pos - cbegin();
    PushBack(elem);
    std::rotate(data_.buf + n, data_.buf + size_ - 1, data_.buf + size_);
    return data_.buf + n;
  }

  iterator Insert(const_iterator pos, T&& elem) {
    size_t n = pos - cbegin();
    PushBack(std::move(elem));
    std::rotate(data_.buf + n, data_.buf + size_ - 1, data_.buf + size_);
    return data_.buf + n;
  }

  // Конструирует элемент по заданным аргументам конструктора перед pos
  // Возвращает итератор на вставленный элемент
  template <typename ... Args>
  iterator Emplace(const_iterator it, Args&&... args) {
    size_t n = it - cbegin();
    EmplaceBack(std::forward<Args>(args)...);
    std::rotate(data_.buf + n, data_.buf + size_ - 1, data_.buf + size_);
    return data_.buf + n;
  }

  // Удаляет элемент на позиции pos
  // Возвращает итератор на элемент, следующий за удалённым
  iterator Erase(const_iterator it) {
    size_t n = it - cbegin();
    if (n + 1 < size_) {
      std::move(data_.buf + n + 1, data_.buf + size_, data_.buf + n);
    }
    PopBack();
    return begin() + n;
  }

private:
  RawMemory<T> data_;
  size_t size_ = 0;
};

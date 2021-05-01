#include <utility>

struct BadOptionalAccess {
};

template <typename T>
class Optional {
private:
  alignas(T) unsigned char data[sizeof(T)];
  bool defined = false;

public:
  Optional() = default;

  Optional(const T& elem) {
    new (data) T(elem);
    defined = true;
  }

  Optional(T&& elem) {
    new (data) T(std::move(elem));
    defined = true;
  }

  Optional(const Optional& other) {
    if (other.defined) {
      new (data) T(*other);
      defined = true;
    }
  }

  Optional(Optional&& other) {
    if (other.defined) {
      new (data) T(std::move(*other));
      defined = true;
    }
  }

  Optional& operator=(const T& elem) {
    if (defined) {
      **this = elem;
    } else {
      new (data) T(elem);
      defined = true;
    }
    return *this;
  }

  Optional& operator=(T&& elem) {
    if (defined) {
      **this = std::move(elem);
    } else {
      new (data) T(std::move(elem));
      defined = true;
    }
    return *this;
  }

  Optional& operator=(const Optional& other) {
    if (other.defined) {
      if (defined) {
        **this = *other;
      } else {
        new (data) T(*other);
      }
    } else if (defined) {
      (*this)->~T();
    }
    defined = other.defined;
    return *this;
  }

  Optional& operator=(Optional&& other) {
    if (other.defined) {
      if (defined) {
        **this = std::move(*other);
      } else {
        new (data) T(std::move(*other));
      }
    } else if (defined) {
      (*this)->~T();
    }
    defined = other.defined;
    return *this;
  }

  bool HasValue() const {
    return defined;
  }

  T& operator*() {
    return *reinterpret_cast<T*>(data);
  }

  const T& operator*() const {
    return *reinterpret_cast<const T*>(data);
  }

  T* operator->() {
    return reinterpret_cast<T*>(data);
  }

  const T* operator->() const {
    return reinterpret_cast<const T*>(data);
  }

  T& Value() {
    if (defined) {
      return *reinterpret_cast<T*>(data);
    }
    throw BadOptionalAccess();
  }

  const T& Value() const {
    if (defined) {
      return *reinterpret_cast<const T*>(data);
    }
    throw BadOptionalAccess();
  }

  void Reset() {
    if (defined) {
      reinterpret_cast<T*>(data)->~T();
      defined = false;
    }
  }

  ~Optional() {
    if (defined) {
      reinterpret_cast<T*>(data)->~T();
    }
  }
};

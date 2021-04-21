#include <utility>

namespace RAII {

template <typename Provider>
class Booking {
 public:
  Booking(Provider* provider, int id);
  ~Booking();

  Booking(const Booking&) = delete;
  Booking(Booking&&);

  Booking& operator=(const Booking&) = delete;
  Booking& operator=(Booking&&);

 private:
  Provider* provider_ = nullptr;
  int id_ = -1;

  void CancelOrComplete();
};

template <typename Provider>
Booking<Provider>::Booking(Provider* provider, int id)
  : provider_(provider)
  , id_(id)
{
}

template <typename Provider>
Booking<Provider>::Booking(Booking&& other) {
  std::swap(provider_, other.provider_);
  std::swap(id_, other.id_);
}

template <typename Provider>
Booking<Provider>& Booking<Provider>::operator=(Booking&& other) {
  CancelOrComplete();
  provider_ = other.provider_;
  other.provider_ = nullptr;
  id_ = other.id_;
  return *this;
}

template <typename Provider>
Booking<Provider>::~Booking() {
  CancelOrComplete();
}

template <typename Provider>
void Booking<Provider>::CancelOrComplete() {
  if (provider_) {
    provider_->CancelOrComplete(*this);
  }
}

}  // namespace RAII
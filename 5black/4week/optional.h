// Исключение этого типа должно генерироваться при обращении к "пустому" Optional в функции Value
struct BadOptionalAccess {
};

template <typename T>
class Optional {
private:
  // alignas нужен для правильного выравнивания блока памяти
  alignas(T) unsigned char data[sizeof(T)];
  bool defined = false;

public:
  Optional() = default;
  Optional(const T& elem);
  Optional(T&& elem);
  Optional(const Optional& other);
  Optional(Optional&& other);

  Optional& operator=(const T& elem);
  Optional& operator=(T&& elem);
  Optional& operator=(const Optional& other);
  Optional& operator=(Optional&& other);

  bool HasValue() const;

  // Эти операторы не должны делать никаких проверок на пустоту.
  // Проверки остаются на совести программиста.
  T& operator*();
  const T& operator*() const;
  T* operator->();
  const T* operator->() const;

  // Генерирует исключение BadOptionalAccess, если объекта нет
  T& Value();
  const T& Value() const;

  void Reset();

  ~Optional();
};

#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <map>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
class PriorityCollection {
public:
  using Id = size_t;
  using Priority = int;

private:
  using DataPart = map<size_t, T>;
  using DataPartIt = typename DataPart::iterator;
  using Data = map<Priority, DataPart>;
  using DataIt = typename Data::iterator;
  using IndexPart = pair<DataIt, DataPartIt>;
  using Index = map<Id, IndexPart>;

public:
  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
    Id id = last_id_++;
    AddImpl(id, move(object), 0);
    return id;
  }

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)
  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end,
           IdOutputIt ids_begin) {
    for (; range_begin != range_end; ++range_begin) {
      *(ids_begin++) = Add(move(*range_begin));
    }
  }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const {
    return index_.find(id) != index_.end();
  }

  // Получить объект по идентификатору
  const T& Get(Id id) const {
    return index_.find(id)->second.second->second;
  }

  // Увеличить приоритет объекта на 1
  void Promote(Id id) {
    auto item = RemImpl(id); 
    AddImpl(id, move(item.first), ++item.second);
  }

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, Priority> GetMax() const {
    auto d_it = --data_.end();
    auto p_it = --d_it->second.end();
    return {p_it->second, d_it->first};
  }

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, Priority> PopMax() {
    auto d_it = --data_.end();
    auto p_it = --d_it->second.end();
    return RemImpl(p_it->first);
  }

private:
  Id last_id_ = 0;
  Data data_;
  Index index_;

  void AddImpl(Id id, T object, Priority priority) {
    auto data_r = data_.try_emplace(priority);
    auto part_r = data_r.first->second.try_emplace(id, move(object));
    index_[id] = {data_r.first, part_r.first};
  }

  pair<T, Priority> RemImpl(Id id) {
    auto its_it = index_.find(id);
    IndexPart& its = its_it->second;
    T object = move(its.second->second);
    Priority priority = its.first->first;

    its.first->second.erase(its.second);
    if (its.first->second.empty()) {
      data_.erase(its.first);
    }

    index_.erase(its_it);

    return {move(object), priority};
  }
};


class StringNonCopyable : public string {
public:
  using string::string;  // Позволяет использовать конструкторы строки
  StringNonCopyable(const StringNonCopyable&) = delete;
  StringNonCopyable(StringNonCopyable&&) = default;
  StringNonCopyable& operator=(const StringNonCopyable&) = delete;
  StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
  PriorityCollection<StringNonCopyable> strings;
  const auto white_id = strings.Add("white");
  const auto yellow_id = strings.Add("yellow");
  const auto red_id = strings.Add("red");

  ASSERT(strings.IsValid(white_id));

  strings.Promote(yellow_id);
  for (int i = 0; i < 2; ++i) {
    strings.Promote(red_id);
  }
  strings.Promote(yellow_id);
  {
    const auto item = strings.GetMax();
    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
    ASSERT(!strings.IsValid(red_id));
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "yellow");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "white");
    ASSERT_EQUAL(item.second, 0);
  }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  return 0;
}

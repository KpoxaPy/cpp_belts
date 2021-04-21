#pragma once

#include "iterator_range.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <iosfwd>
#include <map>
#include <functional>

struct Date {
  int year, month, day;
};

struct Contact {
  std::string name;
  std::optional<Date> birthday;
  std::vector<std::string> phones;
};

class PhoneBook {
  struct PrefixCompare {
    using is_transparent = void;

    bool operator()(const Contact& lhs, const Contact& rhs);

    bool operator()(const Contact& lhs, std::string_view rhs);
    bool operator()(std::string_view lhs, const Contact& rhs);

   private:
    std::less<std::string> less_str;
    std::less<std::string_view> less_view;
  };

  using Contacts = std::vector<Contact>;

public:
  explicit PhoneBook(std::vector<Contact> contacts);

  using ContactRange = IteratorRange<Contacts::const_iterator>;
  ContactRange FindByNamePrefix(std::string_view name_prefix) const;

  void SaveTo(std::ostream& output) const;

private:
  Contacts contacts_;
  PrefixCompare contacts_comparator_;
};

PhoneBook DeserializePhoneBook(std::istream& input);
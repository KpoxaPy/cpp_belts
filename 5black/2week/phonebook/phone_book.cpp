#include "phone_book.h"

#include <algorithm>
#include <iterator>

#include "contact.pb.h"

namespace {

Date FillDate(const PhoneBookSerialize::Date& date) {
  return {date.year(), date.month(), date.day()};
}

Contact FillContact(const PhoneBookSerialize::Contact& contact) {
  Contact c;
  c.name = contact.name();
  if (contact.has_birthday()) {
    c.birthday = FillDate(contact.birthday());
  }
  for (const auto& p : contact.phone_number()) {
    c.phones.push_back(p);
  }
  return c;
}

}

bool PhoneBook::PrefixCompare::operator()(const Contact& lhs, const Contact& rhs) {
  return less_str(lhs.name, rhs.name);
}

bool PhoneBook::PrefixCompare::operator()(const Contact& lhs, std::string_view rhs) {
  return less_view(std::string_view(lhs.name).substr(0, rhs.size()), rhs);
}

bool PhoneBook::PrefixCompare::operator()(std::string_view lhs, const Contact& rhs) {
  return less_view(lhs, std::string_view(rhs.name).substr(0, lhs.size()));
}

PhoneBook::PhoneBook(std::vector<Contact> contacts)
  : contacts_(
    std::make_move_iterator(std::begin(contacts)),
    std::make_move_iterator(std::end(contacts))
  )
{
  std::sort(std::begin(contacts_), std::end(contacts_), contacts_comparator_);
}

PhoneBook::ContactRange PhoneBook::FindByNamePrefix(std::string_view name_prefix) const {
  auto [begin, end] = std::equal_range(
      std::begin(contacts_),
      std::end(contacts_),
      name_prefix,
      contacts_comparator_);
  return {begin, end};
}

void PhoneBook::SaveTo(std::ostream& output) const {
  PhoneBookSerialize::ContactList list;

  for (const auto& c : contacts_) {
    auto new_contact = list.add_contact();
    new_contact->set_name(c.name);
    if (c.birthday) {
      auto bday = new_contact->mutable_birthday();
      bday->set_year(c.birthday->year);
      bday->set_month(c.birthday->month);
      bday->set_day(c.birthday->day);
    }
    for (const auto& p : c.phones) {
      *new_contact->add_phone_number() = p;
    }
  }

  list.SerializeToOstream(&output);
}

PhoneBook DeserializePhoneBook(std::istream& input) {
  std::vector<Contact> v;

  PhoneBookSerialize::ContactList list;
  if (list.ParseFromIstream(&input)) {
    for (const auto& c : list.contact()) {
      v.push_back(FillContact(c));
    }
  }

  return PhoneBook{std::move(v)};
}
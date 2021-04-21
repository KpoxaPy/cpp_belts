#include "test_runner.h"
#include "profile.h"

#include <iostream>
#include <algorithm>
#include <map>
#include <list>
#include <string>
#include <random>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

// Реализуйте этот класс
class Database {
private:
  struct Node;

  using Bank = list<Record>;
  using Index = unordered_map<string, Node>;
  using SIdxUser = multimap<string, Record*>;
  using SIdxTimestamp = multimap<int, Record*>;
  using SIdxKarma = multimap<int, Record*>;
  
  struct Node {
    Bank::iterator it;
    SIdxUser::iterator sidx_user_it;
    SIdxTimestamp::iterator sidx_timestamp_it;
    SIdxKarma::iterator sidx_karma_it;
  };

public:
  bool Put(Record record) {
    auto idx = index_.emplace(record.id, Node{});
    if (!idx.second) {
      return false;
    }
    Node& node = idx.first->second;

    node.it = bank_.emplace(bank_.end(), move(record));
    node.sidx_user_it = users_.emplace(node.it->user, &*node.it);
    node.sidx_timestamp_it = timestamps_.emplace(node.it->timestamp, &*node.it);
    node.sidx_karma_it = karma_.emplace(node.it->karma, &*node.it);
    return true;
  }

  const Record* GetById(const string& id) const {
    auto it = index_.find(id);
    if (it == index_.end()) {
      return nullptr;
    }
    return &*it->second.it;
  }

  bool Erase(const string& id) {
    auto it = index_.find(id);
    if (it == index_.end()) {
      return false;
    }

    bank_.erase(it->second.it);
    users_.erase(it->second.sidx_user_it);
    timestamps_.erase(it->second.sidx_timestamp_it);
    karma_.erase(it->second.sidx_karma_it);
    index_.erase(it);
    return true;
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
    IterateOverCallback(timestamps_.lower_bound(low), timestamps_.upper_bound(high), move(callback));
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    IterateOverCallback(karma_.lower_bound(low), karma_.upper_bound(high), move(callback));
  }

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const {
    auto [first, last] = users_.equal_range(user);
    IterateOverCallback(first, last, move(callback));
  }

private:
  Bank bank_;
  Index index_;
  SIdxUser users_;
  SIdxTimestamp timestamps_;
  SIdxKarma karma_;

  template <typename Callback, typename Iterator>
  void IterateOverCallback(Iterator first, Iterator last, Callback callback) const {
    for (; first != last; ++first) {
      if (!callback(*first->second)) {
        break;
      }
    }
  }
};

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});
  db.Put({"id3", "ololol", "master", 1536107260, 3000});

  ASSERT(!db.Put({"id1", "Don't sell", "master", 1536107260, 1000}));
  ASSERT(!db.Put({"id2", "Rethink life", "master", 1536107260, 2000}));
  ASSERT(!db.Put({"id3", "ololol", "master", 1536107260, 3000}));

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(3, count);

  db.Erase("id2");

  vector<string> result;
  db.AllByUser("master", [&result](const Record& r) {
    result.push_back(r.id);
    return true;
  });
  sort(result.begin(), result.end());

  vector<string> expected = {"id1", "id3"};
  ASSERT_EQUAL(expected, result);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

string GetId(size_t id) {
  ostringstream s;
  s << "id" << id;
  return s.str();
}

// void TestLoad() {
//   const size_t IDS_COUNT = 100'000;
//   const vector<string> NAMES = {
//     "Kieran", "Jong", "Jisheng", "Vickie", "Adam", "Simon", "Lance",
//     "Everett", "Bryan", "Timothy", "Daren", "Emmett", "Edwin", "List",
//     "Sharon", "Trying", "Dan", "Saad", "Kamiya", "Nikolai", "Del",
//     "Casper", "Arthur", "Mac", "Rajesh", "Belinda", "Robin", "Lenora",
//     "Carisa", "Penny", "Sabrina", "Ofer", "Suzanne", "Pria", "Magnus",
//     "Ralph", "Cathrin", "Phill", "Alex", "Reinhard", "Marsh", "Tandy",
//     "Mongo", "Matthieu", "Sundaresan", "Piotr", "Ramneek", "Lynne", "Erwin",
//     "Edgar", "Srikanth", "Kimberly", "Jingbai", "Lui", "Jussi", "Wilmer",
//     "Stuart", "Grant", "Hotta", "Stan", "Samir", "Ramadoss", "Narendra",
//     "Gill", "Jeff", "Raul", "Ken", "Rahul", "Max", "Agatha",
//     "Elizabeth", "Tai", "Ellen", "Matt", "Ian", "Toerless", "Naomi",
//     "Rodent", "Terrance", "Ethan", "Florian", "Rik", "Stanislaw", "Mott",
//     "Charlie", "Marguerite", "Hitoshi", "Panacea", "Dieter", "Randell", "Earle",
//     "Rajiv", "Ted", "Mann", "Bobbie", "Pat", "Olivier", "Harmon",
//     "Raman", "Justin"
//   };

//   const string title = "Today is the first day of the rest of your life!";

// 	std::default_random_engine rd(42);
// 	std::uniform_int_distribution<size_t> ids_gen(0, IDS_COUNT);
// 	std::uniform_int_distribution<size_t> names_gen(0, NAMES.size() - 1);
// 	std::uniform_int_distribution<int> ts_gen(0, IDS_COUNT / 10);
// 	std::uniform_int_distribution<int> karma_gen(0, IDS_COUNT / 10);

//   Database db;

//   {
//     AvgMeter m("Put avg");
//     for (size_t i = 0; i < IDS_COUNT; ++i) {
//       AVG_DURATION(m);
//       db.Put({GetId(ids_gen(rd)), title, NAMES[names_gen(rd)], ts_gen(rd), karma_gen(rd)});
//     }
//   }

//   {
//     AvgMeter m("Erase avg");
//     for (size_t i = 0; i < IDS_COUNT; ++i) {
//       AVG_DURATION(m);
//       db.Erase(GetId(ids_gen(rd)));
//     }
//   }

//   for (size_t i = 0; i < IDS_COUNT; ++i) {
//     db.Put({GetId(i), title, NAMES[names_gen(rd)], ts_gen(rd), karma_gen(rd)});
//   }

//   {
//     AvgMeter m("User idx avg");
//     for (size_t i = 0; i < NAMES.size(); ++i) {
//       AVG_DURATION(m);
//       db.AllByUser(NAMES[i], [](const Record&){ return false; });
//     }
//   }

//   {
//     AvgMeter m("TS idx avg");
//     for (size_t i = 0; i <= 3000; ++i) {
//       for (size_t j = i; j <= 3000; ++j) {
//         AVG_DURATION(m);
//         db.RangeByTimestamp(i, j, [](const Record&){ return false; });
//       }
//     }
//   }
// }

int main() {
  {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
  }
  // TestLoad();
  return 0;
}

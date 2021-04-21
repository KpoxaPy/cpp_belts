#include <iomanip>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utility>

using namespace std;

class ReadingManager {
public:
  ReadingManager()
    : page_ranks_(MAX_PAGES + 1, 0)
  {}

  void Read(int user_id, int page_count) {
    ReadImpl(user_id, page_count);
  }

  double Cheer(int user_id) const {
    return CheerImpl(user_id);
  }

private:
  static const int MAX_PAGES = 1'000;

  struct User {
    int pages;
  };

  map<int, User> users_;
  vector<int> page_ranks_;

  int GetUserCount() const {
    return users_.size();
  }

  void ReadImpl(int user_id, int page_count) {
    auto it = users_.find(user_id);
    if (it != users_.end()) {
      page_ranks_[it->second.pages]--;
      it->second.pages = page_count;
    } else {
      users_[user_id].pages = page_count;
    }
    page_ranks_[page_count]++;
  }

  double CheerImpl(int user_id) const {
    auto it = users_.find(user_id);
    if (it == users_.end()) {
      return 0;
    }

    const User& user = it->second;
    const int user_count = GetUserCount();
    if (user_count == 1) {
      return 1;
    }

    int count = 0;
    for (auto i = page_ranks_.begin(); i < page_ranks_.begin() + user.pages; ++i) {
      count += *i;
    }

    return count * 1.0 / (user_count - 1);
  }
};


int main() {
  // Для ускорения чтения данных отключается синхронизация
  // cin и cout с stdio,
  // а также выполняется отвязка cin от cout
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  ReadingManager manager;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int user_id;
    cin >> user_id;

    if (query_type == "READ") {
      int page_count;
      cin >> page_count;
      manager.Read(user_id, page_count);
    } else if (query_type == "CHEER") {
      cout << setprecision(6) << manager.Cheer(user_id) << "\n";
    }
  }

  return 0;
}
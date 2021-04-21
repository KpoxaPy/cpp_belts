#include <list>
#include <mutex>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

#include "Common.h"

using namespace std;

class LruCache : public ICache {
 private:
  struct Node;

  using Storage = unordered_map<string, Node>;
  using CacheOrder = list<string_view>;

  struct Node {
    CacheOrder::iterator order_it;
    BookPtr book;
  };

 public:
  LruCache(
      shared_ptr<IBooksUnpacker> books_unpacker,
      const Settings& settings)
      : books_unpacker_(books_unpacker), settings_(settings), free_memory_(settings_.max_memory) {}

  BookPtr GetBook(const string& book_name) override {
    if (auto ptr = Find(book_name); ptr) {
      return ptr;
    }
    return TryAdd(book_name);
  }

 private:
  shared_ptr<IBooksUnpacker> books_unpacker_;
  const Settings& settings_;

  mutex mut_;
  size_t free_memory_;

  Storage storage_;
  CacheOrder order_;

  BookPtr Find(const string& n) {
    lock_guard g(mut_);
    auto storage_ptr = storage_.find(n);
    if (storage_ptr == storage_.end()) {
      return nullptr;
    }

    Node& node = storage_ptr->second;
    string_view view = *node.order_it;
    order_.erase(node.order_it);
    order_.push_back(view);
    node.order_it = prev(order_.end());

    return node.book;
  }

  void FreeCacheFor(size_t s) {
    while (!order_.empty() && free_memory_ < s) {
      auto it = storage_.find(string(order_.front()));
      if (it == storage_.end()) {
        throw runtime_error("Failed invariant for LRU Cache");
      }
      free_memory_ += it->second.book->GetContent().size();
      storage_.erase(it);
      order_.pop_front();
    }
  }

  BookPtr TryAdd(const string& n) {
    BookPtr ptr = books_unpacker_->UnpackBook(n);

    {
      lock_guard g(mut_);
      const size_t book_size = ptr->GetContent().size();
      FreeCacheFor(book_size);
      if (book_size <= free_memory_) {
        auto [it, ok] = storage_.insert({n, {{}, ptr}});
        if (ok) {
          string_view view = it->first;
          order_.push_back(view);
          it->second.order_it = prev(order_.end());
        }
        free_memory_ -= book_size;
      }
    }

    return ptr;
  }
};

unique_ptr<ICache> MakeCache(
    shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings& settings) {
  return make_unique<LruCache>(move(books_unpacker), settings);
}

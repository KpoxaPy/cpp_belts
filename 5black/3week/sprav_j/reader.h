#pragma once

#include <istream>
#include <string>
#include <optional>
#include <functional>
#include <utility>

class InputReader {
 public:
  InputReader() = default;

  InputReader& SetUseCounter(bool value) {
    use_counter_ = value;
    return *this;
  }

  template <typename Callback>
  InputReader& SetCallback(Callback cb) {
    callback_.emplace(std::move(cb));
    return *this;
  }

  InputReader& Process(std::istream& s) {
    if (use_counter_) {
      int count;
      std::string line;
      s >> count;
      getline(s, line);
      while (count-- > 0) {
        getline(s, line);
        if (!line.empty() && callback_) {
          callback_.value()(std::move(line));
        }
      }
    } else {
      std::string line;
      while (getline(s, line)) {
        if (!line.empty() && callback_) {
          callback_.value()(std::move(line));
        }
      }
    }
    return *this;
  }

 private:
  bool use_counter_ = true;
  std::optional<std::function<void(std::string)>> callback_;
};
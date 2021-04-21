#pragma once

#include "macro.h"

#include <chrono>
#include <iostream>
#include <string>

using namespace std;
using namespace std::chrono;

template <typename T>
class DurationMeter {
public:
  DurationMeter()
    : start(steady_clock::now())
  {}

  void Start() {
    start = steady_clock::now();
  }

  uint64_t Get() const {
    auto finish = steady_clock::now();
    auto dur = finish - start;
    return duration_cast<T>(dur).count();
  }

private:
  steady_clock::time_point start;
};

template <typename Func>
void AvgMeter(const string& msg, size_t count, Func f) {
  DurationMeter<milliseconds> meter;
  uint64_t sum_ms = 0;
  uint64_t max_ms = 0;
  uint64_t min_ms = numeric_limits<long long>::max();
  for (size_t i = 0; i < count; ++i) {
    meter.Start();
    f();
    uint64_t t = meter.Get();

    if (max_ms < t) max_ms = t;
    if (min_ms > t) min_ms = t;
    sum_ms += t;
  }
  cerr << (msg.empty() ? "" : msg + ": ")
      << sum_ms / count << " ms (min: " << min_ms << " ms; max: " << max_ms << " ms)" << endl;
}

#define AVG_DURATION(message, count, func) \
  AvgMeter(message, count, [&](){ func; })

class LogDuration {
public:
  explicit LogDuration(const string& msg = "")
    : message(msg)
  {}

  ~LogDuration() {
    auto ms = meter.Get();
    cerr << (message.empty() ? "" : message + ": ")
       << ms << " ms" << endl;
  }

private:
  string message;
  DurationMeter<milliseconds> meter;
};

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};

#pragma once

#include "macro.h"

#include <chrono>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace std::chrono;

ostream& operator<<(ostream& s, const nanoseconds& us);
ostream& operator<<(ostream& s, const microseconds& us);
ostream& operator<<(ostream& s, const milliseconds& us);
ostream& operator<<(ostream& s, const seconds& us);

template <typename T>
class DurationMeter {
public:
  DurationMeter()
    : start(steady_clock::now())
  {}

  void Start() {
    start = steady_clock::now();
  }

  T Get() const {
    auto finish = steady_clock::now();
    auto dur = finish - start;
    return duration_cast<T>(dur);
  }

private:
  steady_clock::time_point start;
};

template <typename Duration = microseconds>
class AvgMeter {
public:
  using DurationType = Duration;

  AvgMeter(const string& msg = "")
    : message(msg)
    , sum_ts(0)
  {}

  ~AvgMeter() {
    if (count > 0) {
      sum_ts /= count;
    }
    cerr << (message.empty() ? "" : message + ": ")
       << sum_ts << endl;
  }

  void AddTime(Duration time) {
    ++count;
    sum_ts += time;
  }

private:
  string message;
  size_t count = 0;
  Duration sum_ts;
};

template <typename Duration = microseconds>
class MaxMeter {
public:
  using DurationType = Duration;

  MaxMeter(const string& msg = "")
    : message(msg)
    , max_ts(0)
  {}

  ~MaxMeter() {
    cerr << (message.empty() ? "" : message + ": ")
       << max_ts << endl;
  }

  void AddTime(Duration time) {
    if (max_ts < time) {
      max_ts = time;
    }
  }

private:
  string message;
  Duration max_ts;
};

template <typename Duration = microseconds>
class StatMeter {
public:
  using DurationType = Duration;

  StatMeter(const string& msg = "")
    : message(msg)
  {}

  string Get() const {
    ostringstream ss;
    ss << (message.empty() ? "" : message + ": ")
       << "total " << sum_ts
       << " count " << count
       << " avg " << (count > 0 ? sum_ts / count : Duration(0))
       << " min " << (count > 0 ? min_ts : Duration(0))
       << " max " << max_ts;
    return ss.str();
  }

  void AddTime(Duration time) {
    ++count;
    sum_ts += time;
    if (min_ts > time) {
      min_ts = time;
    }
    if (max_ts < time) {
      max_ts = time;
    }
  }

private:
  string message;
  Duration min_ts = Duration::max();
  Duration max_ts = Duration(0);
  Duration sum_ts = Duration(0);
  int count = 0;
};

template <typename OutputDuration = milliseconds, typename Duration = nanoseconds>
class SumMeter {
public:
  using DurationType = Duration;

  SumMeter(const string& msg = "")
    : message(msg)
    , sum_ts(0)
  {}

  ~SumMeter() {
    cerr << (message.empty() ? "" : message + ": ")
       << duration_cast<OutputDuration>(sum_ts) << endl;
  }

  void AddTime(Duration time) {
    sum_ts += time;
  }

private:
  string message;
  Duration sum_ts;
};

template <typename Type>
class MeterLogger {
public:
  explicit MeterLogger(Type& meter)
    : meter(meter)
  {}

  ~MeterLogger() {
    meter.AddTime(m.Get());
  }

private:
  Type& meter;
  DurationMeter<typename Type::DurationType> m;
};

#define METER_DURATION(meter) \
  MeterLogger UNIQ_ID(__LINE__){meter};

template <typename Duration = milliseconds>
class LogDuration {
public:
  explicit LogDuration(const string& msg = "")
    : message(msg)
  {}

  ~LogDuration() {
    auto ms = meter.Get();
    cerr << (message.empty() ? "" : message + ": ")
       << ms << endl;
  }

private:
  string message;
  DurationMeter<Duration> meter;
};

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <deque>
#include <list>

#include "bus.h"
#include "stop.h"
#include "router.h"

enum class RoutePartType {
  NOOP,
  BUS,
  WAIT
};

class Sprav {
 private:
  using StopNames = std::deque<std::string>;
  using Stops = std::unordered_map<std::string_view, Stop>;

  using BusNames = std::deque<std::string>;
  using Buses = std::unordered_map<std::string_view, Bus>;

  struct RouteExtra {
    RoutePartType type;
    size_t id = 0;
    size_t span_count = 0;
  };

  using Graph = ::Graph::DirectedWeightedGraph<double, RouteExtra>;
  using Router = ::Graph::Router<double, RouteExtra>;
  using RouteInfoOpt = std::optional<typename Router::RouteInfo>;
  using Edge = typename Router::Edge;

 public:
  struct RouteItem {
    RoutePartType type;
    double time;
    std::string_view name;
    size_t span_count = 0;
  };

  class Route : public std::list<RouteItem> {
   private:
    friend class Sprav;

    Route(const Sprav& sprav, RouteInfoOpt info);

   public:
    double GetTotalTime() const;
    operator bool() const;

   private:
    const Sprav& sprav_;
    RouteInfoOpt info_opt_;
  };

 public:
  Sprav() = default;

  void SetRoutingSettings(double bus_wait_time, double bus_velocity);
  void Build();

  template <typename InputIt>
  void AddBus(std::string_view name, InputIt begin, InputIt end, bool is_roundtrip) {
    Bus& b = GetBus(name);
    b.is_roundtrip = is_roundtrip;
    for (; begin != end; ++begin) {
      Stop& s = GetStop(*begin);
      b.stops.emplace_back(s.id);
      s.buses.emplace(b.id);
    }
  }

  Bus& GetBus(std::string_view name);
  const Bus& GetBus(size_t id) const;
  std::string_view GetBusName(size_t id) const;
  const Bus* FindBus(std::string_view name) const;

  void AddStop(std::string_view name, double lat, double lon, const std::unordered_map<std::string, int>& distances);

  Stop& GetStop(std::string_view name);
  const Stop& GetStop(size_t id) const;
  std::string_view GetStopName(size_t id) const;
  const Stop& GetStop(std::string_view name) const;
  const Stop* FindStop(std::string_view name) const;

  Route FindRoute(std::string_view from, std::string_view to) const;

 private:
  StopNames stop_names_;
  Stops stops_;
  BusNames bus_names_;
  mutable Buses buses_;

  double bus_wait_time_ = 1;
  double bus_velocity_ = 1;

  std::shared_ptr<Graph> router_graph_;
  std::shared_ptr<Router> router_;

  void CalcBusStats(Bus& r) const;

  template <typename InputIt>
  void AddBusStops(size_t shift, size_t bus_id, InputIt begin, InputIt end);
  void BuildGraph();

  template <typename InputIt>
  void AddBusStops2(size_t bus_id, InputIt begin, InputIt end);
  void BuildGraph2();

  void BuildRouter();
};

using SpravPtr = std::shared_ptr<Sprav>;
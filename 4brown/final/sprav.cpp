#include "sprav.h"
#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include <algorithm>

#include "string_stream_utils.h"
#include "profile.h"

using namespace std;

namespace {

template <typename Node, typename Container, typename Names>
Node& GetNode(Container& c, Names& n, string_view name) {
  auto it = c.find(name);
  if (it != c.end()) {
    return it->second;
  }

  string_view stored_name = n.emplace_back(string(name));
  Node& node = c[stored_name];
  node.id = n.size() - 1;
  return node;
}

}

Sprav::Route::Route(const Sprav& sprav, RouteInfoOpt info_opt)
  : sprav_(sprav)
  , info_opt_(move(info_opt))
{
  if (info_opt_.has_value()) {
    typename RouteInfoOpt::value_type& info = info_opt_.value();
    std::string_view last_bus = {};
    double bus_total_time = 0;
    size_t bus_span_count = 0;
    for (size_t idx = 0; idx < info.edge_count; ++idx) {
      auto edge = sprav_.router_->GetRouteEdge(info.id, idx);
      switch (edge.extra.type) {
        default:
        case RoutePartType::NOOP:
          break; 
        case RoutePartType::BUS:
        {
          auto name = sprav_.GetBusName(edge.extra.id);
          if (name == last_bus) {
            bus_total_time += edge.weight;
            bus_span_count += edge.extra.span_count;
          } else if (last_bus != name) {
            if (!last_bus.empty()) {
              push_back({RoutePartType::BUS, bus_total_time, last_bus, bus_span_count});
            }
            last_bus = name;
            bus_total_time = edge.weight;
            bus_span_count = edge.extra.span_count;
          }
          break;
        }
        case RoutePartType::WAIT:
          if (!last_bus.empty()) {
            push_back({RoutePartType::BUS, bus_total_time, last_bus, bus_span_count});
            last_bus = {};
            bus_total_time = 0;
            bus_span_count = 0;
          }
          push_back({RoutePartType::WAIT, edge.weight, sprav_.GetStopName(edge.extra.id)});
          break;
      }
    }
    if (!last_bus.empty()) {
      push_back({RoutePartType::BUS, bus_total_time, last_bus, bus_span_count});
    }
  }
}

double Sprav::Route::GetTotalTime() const {
  return info_opt_.has_value() ? info_opt_.value().weight : 0;
}

Sprav::Route::operator bool() const {
  return info_opt_.has_value();
}

Bus& Sprav::GetBus(string_view name) {
  return GetNode<Bus>(buses_, bus_names_, name);
}

const Bus& Sprav::GetBus(size_t id) const {
  return buses_.at(GetBusName(id));
}

string_view Sprav::GetBusName(size_t id) const {
  return bus_names_[id];
}

const Bus* Sprav::FindBus(std::string_view name) const {
  auto it = buses_.find(name);
  if (it == buses_.end()) {
    return nullptr;
  }
  CalcBusStats(it->second);
  return &it->second;
}

void Sprav::SetRoutingSettings(double bus_wait_time, double bus_velocity) {
  bus_wait_time_ = bus_wait_time;
  bus_velocity_ = bus_velocity;
}

void Sprav::Build() {
  BuildGraph2();
  BuildRouter();
}

template <typename InputIt>
void Sprav::AddBusStops(size_t shift, size_t bus_id, InputIt begin, InputIt end) {
  for (auto it = begin; next(it) != end && it != end; ++it) {
    //     (from stop)    .(prev rpoint).
    //        1)--l/s-->  3)<--l/s--
    //           (cur rpoint)
    //        2)<--0--
    //      (to stop*)

    double time = GetStop(*it).DistanceTo(*next(it)) / bus_velocity_;

    // 1
    router_graph_->AddEdge({*it * 2, shift, time, {RoutePartType::BUS, bus_id, 1}});
    // 2
    router_graph_->AddEdge({shift, *next(it) * 2 + 1, 0, {RoutePartType::NOOP}});
    // 3, if we have previous route point
    if (it != begin) {
      router_graph_->AddEdge({shift - 1, shift, time, {RoutePartType::BUS, bus_id, 1}});
    }

    shift += 1;
  }
}

void Sprav::BuildGraph() {
  LOG_DURATION("Sprav::BuildGraph");
  size_t stop_edges = stop_names_.size() * 2;
  size_t route_points = 0;
  for (const auto& bus : buses_) {
    if (bus.second.is_roundtrip) {
      route_points += bus.second.stops.size() - 1;
    } else {
      route_points += 2 * (bus.second.stops.size() - 1);
    }
  }
  router_graph_ = make_shared<Graph>(stop_edges + route_points);

  // Fill stop edges
  // (stop*) --w--> (stop)
  for (size_t i = 0; i < stop_names_.size(); ++i) {
    router_graph_->AddEdge({i*2 + 1, i*2, bus_wait_time_, {RoutePartType::WAIT, i}});
  }

  // Fill route point edges
  size_t route_points_shift = stop_edges;
  for (const auto& bus : buses_) {
    AddBusStops(route_points_shift, bus.second.id, bus.second.stops.begin(), bus.second.stops.end());
    route_points_shift += bus.second.stops.size() - 1;
    if (!bus.second.is_roundtrip) {
      AddBusStops(route_points_shift, bus.second.id, bus.second.stops.rbegin(), bus.second.stops.rend());
      route_points_shift += bus.second.stops.size() - 1;
    }
  }
}

template <typename InputIt>
void Sprav::AddBusStops2(size_t bus_id, InputIt begin, InputIt end) {
  for (auto it_from = begin; it_from != end; ++it_from) {
    double time = 0;
    size_t count = 0;
    for (auto it_to = next(it_from); it_to != end; ++it_to) {
      time += GetStop(*prev(it_to)).DistanceTo(*it_to) / bus_velocity_;
      count += 1;
      router_graph_->AddEdge({*it_from * 2, *it_to * 2 + 1, time, {RoutePartType::BUS, bus_id, count}});
    }
  }
}

void Sprav::BuildGraph2() {
  LOG_DURATION("Sprav::BuildGraph2");
  router_graph_ = make_shared<Graph>(stop_names_.size() * 2);

  // Fill stop edges
  // (stop*) --w--> (stop)
  for (size_t i = 0; i < stop_names_.size(); ++i) {
    router_graph_->AddEdge({i*2 + 1, i*2, bus_wait_time_, {RoutePartType::WAIT, i}});
  }

  // Fill route point edges
  for (const auto& bus : buses_) {
    AddBusStops2(bus.second.id, bus.second.stops.begin(), bus.second.stops.end());
    if (!bus.second.is_roundtrip) {
      AddBusStops2(bus.second.id, bus.second.stops.rbegin(), bus.second.stops.rend());
    }
  }
}

void Sprav::BuildRouter() {
  LOG_DURATION("Sprav::BuildRouter");
  if (!router_graph_) {
    throw runtime_error("Failed to build router: no graph");
  }
  router_ = make_shared<Router>(*router_graph_.get());
}

void Sprav::AddStop(std::string_view name, double lat, double lon, const unordered_map<string, int>& distances) {
  Stop& s = GetStop(name);
  s.lat = lat;
  s.lon = lon;
  s.is_declared = true;

  for (auto [other_name, distance]: distances) {
    Stop& other = GetStop(other_name);
    s.distances[other.id] = distance;

    if (other.distances.find(s.id) == other.distances.end()) {
      other.distances[s.id] = distance;
    }
  }
}

Stop& Sprav::GetStop(string_view name) {
  return GetNode<Stop>(stops_, stop_names_, name);
}

const Stop& Sprav::GetStop(size_t id) const {
  auto name = GetStopName(id);
  return stops_.at(name);
}

std::string_view Sprav::GetStopName(size_t id) const {
  return stop_names_[id];
}

const Stop& Sprav::GetStop(string_view name) const {
  return stops_.at(name);
}

const Stop* Sprav::FindStop(std::string_view name) const {
  auto it = stops_.find(name);
  if (it == stops_.end()) {
    return nullptr;
  }
  return &it->second;
}

Sprav::Route Sprav::FindRoute(string_view from, string_view to) const {
  if (!router_) {
    throw runtime_error("Failed to find route: no router");
  }
  return {*this, router_->BuildRoute(GetStop(from).id * 2 + 1, GetStop(to).id * 2 + 1)};
}

void Sprav::CalcBusStats(Bus& b) const {
  if (b.has_stats) {
    return;
  }

  b.stops_count = b.is_roundtrip ? b.stops.size() : b.stops.size() * 2 - 1;
  b.unique_stops_count = unordered_set<size_t>(b.stops.begin(), b.stops.end()).size();

  double curve_length = 0;
  b.length = 0;
  b.curvature = 1;

  if (b.stops.size() > 1) {
    for (auto it = ++b.stops.begin(); it != b.stops.end(); ++it) {
      const Stop& from = GetStop(*prev(it));
      const Stop& to = GetStop(*it);
      b.length += from.DistanceTo(to.id);
      curve_length += GetDistance(from, to);
    }
    if (!b.is_roundtrip) {
      curve_length *= 2;

      for (auto it = ++b.stops.rbegin(); it != b.stops.rend(); ++it) {
        b.length += GetStop(*prev(it)).DistanceTo(*it);
      }
    }
    b.curvature = b.length / curve_length;
  }
}
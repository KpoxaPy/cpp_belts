#include "sprav_impl.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

#include "profile.h"
#include "sprav_mapper.h"
#include "string_stream_utils.h"

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
  node.name = stored_name;
  return node;
}

}

Sprav::PImpl::PImpl(Sprav* sprav)
  : sprav_(sprav) {}

void Sprav::PImpl::SetRoutingSettings(RoutingSettings s) {
  routing_settings_ = move(s);
}

void Sprav::PImpl::SetRenderSettings(RenderSettings s) {
  render_settings_ = move(s);
}

void Sprav::PImpl::Build() {
  BuildGraph();
  BuildRouter();
}

void Sprav::PImpl::AddStop(std::string_view name, double lat, double lon, const unordered_map<string, int>& distances) {
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

void Sprav::PImpl::AddBus(std::string_view name, const std::list<std::string> stops, bool is_roundtrip) {
  Bus& b = GetBus(name);
  b.is_roundtrip = is_roundtrip;
  for (const auto& stop_name : stops) {
    Stop& s = GetStop(stop_name);
    b.stops.emplace_back(s.id);
    s.buses.emplace(b.id);
  }
}

Stop& Sprav::PImpl::GetStop(string_view name) {
  return GetNode<Stop>(stops_, stop_names_, name);
}

Bus& Sprav::PImpl::GetBus(string_view name) {
  return GetNode<Bus>(buses_, bus_names_, name);
}

const Stop& Sprav::PImpl::GetStop(size_t id) const {
  return stops_.at(stop_names_[id]);
}

const Bus& Sprav::PImpl::GetBus(size_t id) const {
  return buses_.at(bus_names_[id]);
}

const Stop* Sprav::PImpl::FindStop(std::string_view name) const {
  auto it = stops_.find(name);
  if (it == stops_.end()) {
    return nullptr;
  }
  return &it->second;
}

const Sprav::Router* Sprav::PImpl::GetRouter() const {
  return router_.get();
}

const Bus* Sprav::PImpl::FindBus(std::string_view name) const {
  auto it = buses_.find(name);
  if (it == buses_.end()) {
    return nullptr;
  }
  CalcBusStats(it->second);
  return &it->second;
}

Sprav::Route Sprav::PImpl::FindRoute(string_view from, string_view to) const {
  if (!router_) {
    throw runtime_error("Failed to find route: no router");
  }
  return {*sprav_, router_->BuildRoute(FindStop(from)->id * 2 + 1, FindStop(to)->id * 2 + 1)};
}

std::string Sprav::PImpl::GetMap() const {
  return GetMapper().Render();
}

std::string Sprav::PImpl::GetRouteMap(const Route& route) const {
  return GetMapper().RenderForRoute(route);
}

template <typename InputIt>
void Sprav::PImpl::AddBusStops(size_t bus_id, InputIt begin, InputIt end) {
  for (auto it_from = begin; it_from != end; ++it_from) {
    double time = 0;
    size_t count = 0;
    for (auto it_to = next(it_from); it_to != end; ++it_to) {
      time += GetStop(*prev(it_to)).DistanceTo(*it_to) / routing_settings_.bus_velocity;
      count += 1;
      router_graph_->AddEdge({*it_from * 2, *it_to * 2 + 1, time, {RoutePartType::BUS, bus_id, count}});
    }
  }
}

void Sprav::PImpl::BuildGraph() {
  LOG_DURATION("Sprav::BuildGraph");
  router_graph_ = make_shared<Graph>(stop_names_.size() * 2);

  // Fill stop edges
  // (stop*) --w--> (stop)
  for (size_t i = 0; i < stop_names_.size(); ++i) {
    router_graph_->AddEdge({i*2 + 1, i*2, routing_settings_.bus_wait_time, {RoutePartType::WAIT, i}});
  }

  // Fill route point edges
  for (const auto& bus : buses_) {
    AddBusStops(bus.second.id, bus.second.stops.begin(), bus.second.stops.end());
    if (!bus.second.is_roundtrip) {
      AddBusStops(bus.second.id, bus.second.stops.rbegin(), bus.second.stops.rend());
    }
  }
}

void Sprav::PImpl::BuildRouter() {
  LOG_DURATION("Sprav::BuildRouter");
  if (!router_graph_) {
    throw runtime_error("Failed to build router: no graph");
  }
  router_ = make_shared<Router>(*router_graph_.get());
}

void Sprav::PImpl::CalcBusStats(Bus& b) const {
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

SpravMapper& Sprav::PImpl::GetMapper() const {
  if (!mapper_) {
    mapper_ = make_shared<SpravMapper>(render_settings_, sprav_, stop_names_, bus_names_);
  }
  return *mapper_;
}
#include "sprav_impl.h"

#include <google/protobuf/arena.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

#include "profile.h"
#include "sprav_mapper.h"
#include "string_stream_utils.h"
#include "transport_catalog.pb.h"

using namespace std;

namespace {

template <typename Node, typename Container, typename Names>
Node& GetNode(Container& c, Names& n, string_view name, optional<Node> pre_node) {
  auto it = c.find(name);
  if (it != c.end()) {
    return it->second;
  }

  if (pre_node) {
    string_view stored_name = n[pre_node.value().id] = string(name);
    Node& node = c[stored_name] = std::move(pre_node.value());
    node.name = stored_name;
    return node;
  }

  string_view stored_name = n.emplace_back(string(name));
  Node& node = c[stored_name];
  node.id = n.size() - 1;
  node.name = stored_name;
  return node;
}

struct Catalog {
  SpravSerialize::TransportCatalog* p;
  google::protobuf::Arena arena;

  Catalog() {
    p = google::protobuf::Arena::CreateMessage<SpravSerialize::TransportCatalog>(&arena);
  }

  ~Catalog() {
    LOG_DURATION("Transport catalog destruct");
    arena.Reset();
  }
};

Catalog catalog;

}

Sprav::PImpl::PImpl(Sprav* sprav)
  : sprav_(sprav) {}

void Sprav::PImpl::Serialize() {
  LOG_DURATION("Sprav::Serialize");

  {
    LOG_DURATION("Sprav::Serialize stops");
    for (const auto& [_, stop] : stops_) {
      stop.SerializeTo(*catalog.p->add_stop());
    }
  }

  {
    LOG_DURATION("Sprav::Serialize buses");
    for (const auto& [_, bus] : buses_) {
      bus.SerializeTo(*catalog.p->add_bus());
    }
  }

  {
    LOG_DURATION("Sprav::Serialize graph");
    router_graph_->Serialize(*catalog.p->mutable_graph());
  }

  {
    LOG_DURATION("Sprav::Serialize router");
    router_->Serialize(*catalog.p->mutable_router());
  }

  {
    LOG_DURATION("Sprav::Serialize render settings");
    render_settings_.Serialize(*catalog.p->mutable_render_settings());
  }

  {
    LOG_DURATION("Sprav::Serialize mapper");
    mapper_->Serialize(*catalog.p->mutable_mapper());
  }

  {
    LOG_DURATION("Sprav::Serialize pages");
    pages_->Serialize(*catalog.p->mutable_pages());
  }

  {
    LOG_DURATION("Sprav::Serialize materialization");
    ofstream ofile(serialization_settings_.file, ios::binary | ios::trunc);
    catalog.p->SerializeToOstream(&ofile);
  }
}


void Sprav::PImpl::Deserialize() {
  LOG_DURATION("Sprav::Deserialize");

  {
    LOG_DURATION("Sprav::Deserialize parsing");
    ifstream ifile(serialization_settings_.file, ios::binary);
    catalog.p->ParseFromIstream(&ifile);
  }

  {
    LOG_DURATION("Sprav::Deserialize render settings");
    render_settings_ = RenderSettings(catalog.p->render_settings());
  }

  {
    LOG_DURATION("Sprav::Deserialize stops");
    stop_names_.resize(catalog.p->stop().size());
    for (const auto& stop : catalog.p->stop()) {
      AddStop(stop.name(), Stop::Parse(stop));
    }
  }

  {
    LOG_DURATION("Sprav::Deserialize buses");
    bus_names_.resize(catalog.p->bus().size());
    for (const auto& bus : catalog.p->bus()) {
      AddBus(bus.name(), Bus::Parse(bus));
    }
  }

  {
    LOG_DURATION("Sprav::Deserialize graph");
    router_graph_ = make_shared<Graph>(catalog.p->graph());
  }

  {
    LOG_DURATION("Sprav::Deserialize router");
    router_ = make_shared<Router>(*router_graph_.get(), catalog.p->router());
  }

  {
    LOG_DURATION("Sprav::Deserialize mapper");
    mapper_ = make_shared<SpravMapper>(sprav_, catalog.p->mapper());
  }

  {
    LOG_DURATION("Sprav::Deserialize pages");
    pages_ = make_shared<Pages>(catalog.p->pages());
  }
}

void Sprav::PImpl::SetSerializationSettings(SerializationSettings s) {
  serialization_settings_ = move(s);
}

void Sprav::PImpl::SetRoutingSettings(RoutingSettings s) {
  routing_settings_ = move(s);
}

void Sprav::PImpl::SetRenderSettings(RenderSettings s) {
  render_settings_ = move(s);
}

const RenderSettings& Sprav::PImpl::GetRenderSettings() const {
  return render_settings_;
}

void Sprav::PImpl::SetPages(PagesPtr pages) {
  pages_ = std::move(pages);
}

PagesPtr Sprav::PImpl::GetPages() const {
  return pages_;
}

void Sprav::PImpl::BuildBase() {
  LOG_DURATION("Sprav::BuildBase");
  for (auto& [_, bus] : buses_) {
    CalcBusStats(bus);
  }
  BuildGraph();
  BuildRouter();

  mapper_ = make_shared<SpravMapper>(sprav_);
}

void Sprav::PImpl::AddStop(std::string_view name, double lat, double lon, const unordered_map<string, int>& distances) {
  Stop& s = GetStop(name);
  s.lat = lat;
  s.lon = lon;

  for (auto [other_name, distance]: distances) {
    Stop& other = GetStop(other_name);
    s.distances[other.id] = distance;

    if (other.distances.find(s.id) == other.distances.end()) {
      other.distances[s.id] = distance;
    }
  }
}

void Sprav::PImpl::AddStop(std::string_view name, Stop&& stop) {
  GetStop(name, std::move(stop));
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

void Sprav::PImpl::AddBus(std::string_view name, Bus&& bus) {
  GetBus(name, std::move(bus));
}

Stop& Sprav::PImpl::GetStop(string_view name, optional<Stop> pre_node) {
  return GetNode<Stop>(stops_, stop_names_, name, pre_node);
}

Bus& Sprav::PImpl::GetBus(string_view name, optional<Bus> pre_node) {
  return GetNode<Bus>(buses_, bus_names_, name, pre_node);
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

Sprav::Router* Sprav::PImpl::GetRouter() const {
  return router_.get();
}

const Bus* Sprav::PImpl::FindBus(std::string_view name) const {
  auto it = buses_.find(name);
  if (it == buses_.end()) {
    return nullptr;
  }
  return &it->second;
}

const Sprav::StopNames& Sprav::PImpl::GetStopNames() const {
  return stop_names_;
}

const Sprav::BusNames& Sprav::PImpl::GetBusNames() const {
  return bus_names_;
}

Sprav::Route Sprav::PImpl::FindRoute(string_view from, string_view to) const {
  if (!router_) {
    throw runtime_error("Failed to find route: no router");
  }

  auto route = router_->BuildRoute(FindStop(from)->id * 2 + 1, FindStop(to)->id * 2 + 1);
  return {*sprav_, std::move(route), Time()};
}

Sprav::Route Sprav::PImpl::FindRouteToCompany(std::string_view from, const YellowPages::Query& query, const Time& time) const {
  using Info = Sprav::Router::RouteInfo;

  if (!router_) {
    throw runtime_error("Failed to find route: no router");
  }

  const auto companies = pages_->Process(query);
  if (companies.empty()) {
    return {*sprav_, {}, time};
  }

  deque<Info> route_candidates;
  deque<pair<size_t, double>> route_times;
  const size_t stop_vid = FindStop(from)->id * 2 + 1;
  for (size_t id : companies) {
    const size_t company_vid = stop_names_.size() * 2 + id;
    auto route_opt = router_->BuildRoute(stop_vid, company_vid);
    if (route_opt) {
      auto wait_opt = pages_->GetWaitTime(id, time + route_opt->weight);
      route_times.push_back({route_candidates.size(), route_opt->weight + wait_opt.value_or(0)});
      route_candidates.push_back(std::move(route_opt.value()));
    }
  }
  assert(route_times.size() == route_candidates.size());

  if (route_candidates.empty()) {
    return {*sprav_, {}, time};
  }

  auto it = std::min_element(begin(route_times), end(route_times),
    [](const auto& lhs, const auto& rhs){
      return lhs.second < rhs.second;
    }
  );

  Info& winner = route_candidates[it->first];
  for (auto& c : route_candidates) {
    if (c.id != winner.id) {
      sprav_->GetRouter()->ReleaseRoute(c.id);
    }
  }

  return {*sprav_, std::move(winner), time};
}

std::string Sprav::PImpl::GetMap() const {
  return GetMapper().Render();
}

std::string Sprav::PImpl::GetRouteMap(const Route& route) const {
  return GetMapper().RenderForRoute(route);
}

Pages::Companies Sprav::PImpl::FindCompanies(const YellowPages::Query& query) {
  return pages_->Process(query);
}

template <typename InputIt>
void Sprav::PImpl::AddBusStops(size_t bus_id, InputIt begin, InputIt end) {
  for (auto it_from = begin; it_from != end; ++it_from) {
    double time = 0;
    size_t count = 0;
    std::list<size_t> stops;
    stops.push_back(*it_from);
    for (auto it_to = next(it_from); it_to != end; ++it_to) {
      time += GetStop(*prev(it_to)).DistanceTo(*it_to) / routing_settings_.bus_velocity;
      count += 1;
      stops.push_back(*it_to);
      router_graph_->AddEdge({*it_from * 2, *it_to * 2 + 1, time, {RoutePartType::RIDE_BUS, bus_id, count, 0, stops}});
    }
  }
}

void Sprav::PImpl::AddCompany(size_t id, const YellowPages::Company& company) {
  const size_t vertex_id = stop_names_.size() * 2 + id;
  for (auto& s : company.nearby_stops()) {
    auto& stop = GetStop(s.name());
    const double time = s.meters() / routing_settings_.pedestrian_velocity;

    router_graph_->AddEdge({stop.id * 2 + 1, vertex_id, time,
      {RoutePartType::WALK_TO_COMPANY, stop.id, 0, id, {}}}
    );
  }
}

void Sprav::PImpl::BuildGraph() {
  LOG_DURATION("Sprav::BuildGraph");

  const size_t stops_border = stop_names_.size() * 2;
  const size_t companies_border = stops_border + pages_->Size();

  router_graph_ = make_shared<Graph>(companies_border);

  // Fill stop edges
  // (stop*) --w--> (stop)
  for (size_t i = 0; i < stop_names_.size(); ++i) {
    router_graph_->AddEdge({i*2 + 1, i*2, routing_settings_.bus_wait_time, {RoutePartType::WAIT_BUS, i}});
  }

  // Fill route point edges
  for (const auto& [_, bus] : buses_) {
    AddBusStops(bus.id, bus.stops.begin(), bus.stops.end());
    if (!bus.is_roundtrip) {
      AddBusStops(bus.id, bus.stops.rbegin(), bus.stops.rend());
    }
  }

  // Fill company edges
  for (size_t id = 0; id < pages_->Size(); ++id) {
    AddCompany(id, pages_->Get(id));
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
  return *mapper_;
}
#include "sprav.h"

#include "sprav_impl.h"

using namespace std;

namespace {

using SerializedExtra = SpravSerialize::Graph::Edge;

SerializedExtra::Type SerializeType(RoutePartType t) {
  switch (t) {
  case RoutePartType::BUS:
    return SerializedExtra::Type::Graph_Edge_Type_BUS;
  case RoutePartType::WAIT:
    return SerializedExtra::Type::Graph_Edge_Type_WAIT;
  default:
    return SerializedExtra::Type::Graph_Edge_Type_NOOP;
  }
}

RoutePartType SerializeType(SerializedExtra::Type t) {
  switch (t) {
  case SerializedExtra::Type::Graph_Edge_Type_BUS:
    return RoutePartType::BUS;
  case SerializedExtra::Type::Graph_Edge_Type_WAIT:
    return RoutePartType::WAIT;
  default:
    return RoutePartType::NOOP;
  }
}

} // namespace

void Sprav::RouteExtra::Serialize(SerializedExtra& m) const {
  m.set_type(SerializeType(type));
  m.set_id(id);
  m.set_span_count(span_count);
  // for (auto stop : stops) {
  //   m.mutable_stops()->Add(stop);
  // }
}

Sprav::RouteExtra Sprav::RouteExtra::ParseFrom(const SerializedExtra& m) {
  RouteExtra e;
  e.type = SerializeType(m.type());
  e.id = m.id();
  e.span_count = m.span_count();
  // e.stops.assign(m.stops().begin(), m.stops().end());
  return e;
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
    std::list<size_t> bus_stops;
    for (size_t idx = 0; idx < info.edge_count; ++idx) {
      auto edge = sprav_.GetRouter()->GetRouteEdge(info.id, idx);
      switch (edge.extra.type) {
        default:
        case RoutePartType::NOOP:
          break; 
        case RoutePartType::BUS:
        {
          auto name = sprav_.GetBus(edge.extra.id).name;
          if (name == last_bus) {
            bus_total_time += edge.weight;
            bus_span_count += edge.extra.span_count;
            // bus_stops.insert(bus_stops.end(), edge.extra.stops.begin(), edge.extra.stops.end());
          } else if (last_bus != name) {
            if (!last_bus.empty()) {
              push_back({RoutePartType::BUS, bus_total_time, last_bus, bus_span_count, bus_stops});
            }
            last_bus = name;
            bus_total_time = edge.weight;
            bus_span_count = edge.extra.span_count;
            // bus_stops.assign(edge.extra.stops.begin(), edge.extra.stops.end());
          }
          break;
        }
        case RoutePartType::WAIT:
          if (!last_bus.empty()) {
            push_back({RoutePartType::BUS, bus_total_time, last_bus, bus_span_count, bus_stops});
            last_bus = {};
            bus_total_time = 0;
            bus_span_count = 0;
          }
          push_back({RoutePartType::WAIT, edge.weight, sprav_.GetStop(edge.extra.id).name});
          break;
      }
    }
    if (!last_bus.empty()) {
      push_back({RoutePartType::BUS, bus_total_time, last_bus, bus_span_count, bus_stops});
    }
    sprav_.GetRouter()->ReleaseRoute(info.id);
  }
}

double Sprav::Route::GetTotalTime() const {
  return info_opt_.has_value() ? info_opt_.value().weight : 0;
}

Sprav::Route::operator bool() const {
  return info_opt_.has_value();
}

Sprav::Sprav()
    : pimpl_(make_unique<PImpl>(this)) {}

Sprav::~Sprav() = default;

void Sprav::Serialize() {
  Pimpl()->Serialize();
}

void Sprav::Deserialize() {
  Pimpl()->Deserialize();
}

void Sprav::SetSerializationSettings(SerializationSettings s) {
  Pimpl()->SetSerializationSettings(move(s));
}

void Sprav::SetRoutingSettings(RoutingSettings s) {
  Pimpl()->SetRoutingSettings(move(s));
}

void Sprav::SetRenderSettings(RenderSettings s) {
  Pimpl()->SetRenderSettings(move(s));
}

void Sprav::AddStop(std::string_view name, double lat, double lon, const std::unordered_map<std::string, int>& distances) {
  Pimpl()->AddStop(move(name), lat, lon, distances);
}

void Sprav::AddBus(std::string_view name, const std::list<std::string> stops, bool is_roundtrip) {
  Pimpl()->AddBus(move(name), stops, is_roundtrip);
}

void Sprav::BuildBase() {
  Pimpl()->BuildBase();
}

const Stop& Sprav::GetStop(size_t id) const {
  return Pimpl()->GetStop(id);
}

const Bus& Sprav::GetBus(size_t id) const {
  return Pimpl()->GetBus(id);
}

const Stop* Sprav::FindStop(std::string_view name) const {
  return Pimpl()->FindStop(name);  
}

const Bus* Sprav::FindBus(std::string_view name) const {
  return Pimpl()->FindBus(name);
}
 
Sprav::Router* Sprav::GetRouter() const {
  return Pimpl()->GetRouter();
}

Sprav::Route Sprav::FindRoute(std::string_view from, std::string_view to) const {
  return Pimpl()->FindRoute(from, to);
}

std::string Sprav::GetMap() const {
  return Pimpl()->GetMap();
}

std::string Sprav::GetRouteMap(const Route& route) const {
  return Pimpl()->GetRouteMap(route);
}

const Sprav::PImpl* Sprav::Pimpl() const {
  return pimpl_.get();
}

Sprav::PImpl* Sprav::Pimpl() {
  return pimpl_.get();
}
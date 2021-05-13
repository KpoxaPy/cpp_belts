#include "sprav.h"

#include "sprav_impl.h"

using namespace std;

namespace {

using SerializedExtra = SpravSerialize::Graph::Edge;

SerializedExtra::Type SerializeType(RoutePartType t) {
  switch (t) {
  case RoutePartType::RIDE_BUS:
    return SerializedExtra::Type::Graph_Edge_Type_RIDE_BUS;
  case RoutePartType::WAIT_BUS:
    return SerializedExtra::Type::Graph_Edge_Type_WAIT_BUS;
  case RoutePartType::WALK_TO_COMPANY:
    return SerializedExtra::Type::Graph_Edge_Type_WALK_TO_COMPANY;
  default:
    return SerializedExtra::Type::Graph_Edge_Type_NOOP;
  }
}

RoutePartType SerializeType(SerializedExtra::Type t) {
  switch (t) {
  case SerializedExtra::Type::Graph_Edge_Type_RIDE_BUS:
    return RoutePartType::RIDE_BUS;
  case SerializedExtra::Type::Graph_Edge_Type_WAIT_BUS:
    return RoutePartType::WAIT_BUS;
  case SerializedExtra::Type::Graph_Edge_Type_WALK_TO_COMPANY:
    return RoutePartType::WALK_TO_COMPANY;
  default:
    return RoutePartType::NOOP;
  }
}

} // namespace

void Sprav::RouteExtra::Serialize(SerializedExtra& m) const {
  m.set_type(SerializeType(type));
  m.set_id(id);
  m.set_company_id(company_id);
  m.set_span_count(span_count);
  for (auto stop : stops) {
    m.mutable_stops()->Add(stop);
  }
}

Sprav::RouteExtra Sprav::RouteExtra::ParseFrom(const SerializedExtra& m) {
  RouteExtra e;
  e.type = SerializeType(m.type());
  e.id = m.id();
  e.company_id = m.company_id();
  e.span_count = m.span_count();
  e.stops.assign(m.stops().begin(), m.stops().end());
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
        case RoutePartType::RIDE_BUS:
        {
          auto name = sprav_.GetBus(edge.extra.id).name;
          if (name == last_bus) {
            bus_total_time += edge.weight;
            bus_span_count += edge.extra.span_count;
            bus_stops.insert(bus_stops.end(), edge.extra.stops.begin(), edge.extra.stops.end());
          } else if (last_bus != name) {
            if (!last_bus.empty()) {
              push_back({RoutePartType::RIDE_BUS, bus_total_time, last_bus, {}, bus_span_count, bus_stops});
            }
            last_bus = name;
            bus_total_time = edge.weight;
            bus_span_count = edge.extra.span_count;
            bus_stops.assign(edge.extra.stops.begin(), edge.extra.stops.end());
          }
          break;
        }
        case RoutePartType::WAIT_BUS:
          if (!last_bus.empty()) {
            push_back({RoutePartType::RIDE_BUS, bus_total_time, last_bus, {}, bus_span_count, bus_stops});
            last_bus = {};
            bus_total_time = 0;
            bus_span_count = 0;
          }
          push_back({RoutePartType::WAIT_BUS, edge.weight, sprav_.GetStop(edge.extra.id).name, {}});
          break;
        case RoutePartType::WALK_TO_COMPANY:
          if (!last_bus.empty()) {
            push_back({RoutePartType::RIDE_BUS, bus_total_time, last_bus, {}, bus_span_count, bus_stops});
          }
          push_back({
            RoutePartType::WALK_TO_COMPANY,
            edge.weight,
            sprav_.GetStop(edge.extra.id).name,
            sprav_.GetPages()->GetCompanyMainName(edge.extra.company_id)
          });
          break;
      }
    }
    if (!last_bus.empty()) {
      push_back({RoutePartType::RIDE_BUS, bus_total_time, last_bus, {}, bus_span_count, bus_stops});
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

Json::Node Sprav::Route::AsJson() const {
  Json::Array items;
  for (auto part : *this) {
    if (part.type == RoutePartType::NOOP) {
      continue;
    }

    Json::Dict item_dict;
    if (part.type == RoutePartType::WAIT_BUS) {
      item_dict["type"] = "WaitBus";
      item_dict["time"] = part.time;
      item_dict["stop_name"] = string(part.name);
    } else if (part.type == RoutePartType::RIDE_BUS) {
      item_dict["type"] = "RideBus";
      item_dict["time"] = part.time;
      item_dict["bus"] = string(part.name);
      item_dict["span_count"] = part.span_count;
    } else if (part.type == RoutePartType::WALK_TO_COMPANY) {
      item_dict["type"] = "WalkToCompany";
      item_dict["time"] = part.time;
      item_dict["stop_name"] = string(part.name);
      item_dict["company"] = string(part.company_name);
    }
    items.push_back(move(item_dict));
  }
  return items;
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

const RenderSettings& Sprav::GetRenderSettings() const {
  return Pimpl()->GetRenderSettings();
}

void Sprav::SetPages(PagesPtr pages) {
  Pimpl()->SetPages(std::move(pages));
}

PagesPtr Sprav::GetPages() const {
  return Pimpl()->GetPages();
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

const Sprav::StopNames& Sprav::GetStopNames() const {
  return Pimpl()->GetStopNames();
}

const Sprav::BusNames& Sprav::GetBusNames() const {
  return Pimpl()->GetBusNames();
}
 
Sprav::Router* Sprav::GetRouter() const {
  return Pimpl()->GetRouter();
}

Sprav::Route Sprav::FindRoute(std::string_view from, std::string_view to) const {
  return Pimpl()->FindRoute(from, to);
}

Sprav::Route Sprav::FindRouteToCompany(std::string_view from, const YellowPages::Query& query) const {
  return Pimpl()->FindRouteToCompany(from, query);
}

std::string Sprav::GetMap() const {
  return Pimpl()->GetMap();
}

std::string Sprav::GetRouteMap(const Route& route) const {
  return Pimpl()->GetRouteMap(route);
}

Pages::Companies Sprav::FindCompanies(const YellowPages::Query& query) {
  return Pimpl()->FindCompanies(query);
}

const Sprav::PImpl* Sprav::Pimpl() const {
  return pimpl_.get();
}

Sprav::PImpl* Sprav::Pimpl() {
  return pimpl_.get();
}
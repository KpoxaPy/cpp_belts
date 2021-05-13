#pragma once

#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "bus.h"
#include "database_queries.pb.h"
#include "pages.h"
#include "render_settings.h"
#include "router.h"
#include "routing_settings.h"
#include "serialization_settings.h"
#include "stop.h"

enum class RoutePartType {
  NOOP,
  RIDE_BUS,
  WAIT_BUS,
  WALK_TO_COMPANY
};

class Sprav {
 private:
  struct RouteExtra {
    RoutePartType type;
    size_t id = 0;
    size_t span_count = 0;
    size_t company_id = 0;
    std::list<size_t> stops = {};

    void Serialize(SpravSerialize::Graph::Edge& m) const;
    static RouteExtra ParseFrom(const SpravSerialize::Graph::Edge& m);
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
    size_t company_id = 0;
    size_t span_count = 0;

    std::list<size_t> stops = {};
  };

  class Route : public std::list<RouteItem> {
   private:
    friend class Sprav;
    Route(const Sprav& sprav, RouteInfoOpt info);

   public:
    double GetTotalTime() const;
    operator bool() const;

    Json::Node AsJson() const;

   private:
    const Sprav& sprav_;
    RouteInfoOpt info_opt_;
  };

  using StopNames = std::deque<std::string>;
  using BusNames = std::deque<std::string>;

 public:
  Sprav();
  ~Sprav();

  void Serialize();
  void Deserialize();

  void SetSerializationSettings(SerializationSettings s);

  void SetRoutingSettings(RoutingSettings s);

  void SetRenderSettings(RenderSettings s);
  const RenderSettings& GetRenderSettings() const;

  void SetPages(PagesPtr pages);
  PagesPtr GetPages() const;

  void AddStop(std::string_view name, double lat, double lon, const std::unordered_map<std::string, int>& distances);
  void AddBus(std::string_view name, const std::list<std::string> stops, bool is_roundtrip);

  void BuildBase();

  const Stop& GetStop(size_t id) const;
  const Bus& GetBus(size_t id) const;

  const Stop* FindStop(std::string_view name) const;
  const Bus* FindBus(std::string_view name) const;

  const StopNames& GetStopNames() const;
  const BusNames& GetBusNames() const;

  Router* GetRouter() const;
  Route FindRoute(std::string_view from, std::string_view to) const;
  Route FindRouteToCompany(std::string_view from, const YellowPages::Query& query) const;

  std::string GetMap() const;
  std::string GetRouteMap(const Route& route) const;

  Pages::Companies FindCompanies(const YellowPages::Query& query);

 private:
  class PImpl;
  std::unique_ptr<PImpl> pimpl_;

  const PImpl* Pimpl() const;
  PImpl* Pimpl();
};

using SpravPtr = std::shared_ptr<Sprav>;
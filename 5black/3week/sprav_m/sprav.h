#pragma once

#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "bus.h"
#include "render_settings.h"
#include "router.h"
#include "routing_settings.h"
#include "stop.h"

enum class RoutePartType {
  NOOP,
  BUS,
  WAIT
};

class PointCoordAdapter;
class Paletter;

class Sprav {
 private:
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
  Sprav();
  ~Sprav();

  void SetRoutingSettings(RoutingSettings s);
  void SetRenderSettings(RenderSettings s);

  void AddStop(std::string_view name, double lat, double lon, const std::unordered_map<std::string, int>& distances);
  void AddBus(std::string_view name, const std::list<std::string> stops, bool is_roundtrip);

  void Build();

  const Stop& GetStop(size_t id) const;
  const Bus& GetBus(size_t id) const;

  const Stop* FindStop(std::string_view name) const;
  const Bus* FindBus(std::string_view name) const;

  const Router* GetRouter() const;
  Route FindRoute(std::string_view from, std::string_view to) const;

  std::string GetMap() const;

 private:
  class PImpl;
  std::unique_ptr<PImpl> pimpl_;

  const PImpl* Pimpl() const;
  PImpl* Pimpl();
};

using SpravPtr = std::shared_ptr<Sprav>;
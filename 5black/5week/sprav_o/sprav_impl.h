#pragma once

#include "sprav.h"
#include "sprav_mapper.h"

class Sprav::PImpl {
 private:
  using StopNames = std::deque<std::string>;
  using Stops = std::unordered_map<std::string_view, Stop>;

  using BusNames = std::deque<std::string>;
  using Buses = std::unordered_map<std::string_view, Bus>;

 public:
  PImpl(Sprav* sprav);

  void Serialize();
  void DeSerialize();

  void SetSerializationSettings(SerializationSettings s);
  void SetRoutingSettings(RoutingSettings s);
  void SetRenderSettings(RenderSettings s);

  void BuildBase();
  void Build();

  void AddStop(std::string_view name, double lat, double lon, const std::unordered_map<std::string, int>& distances);
  void AddStop(std::string_view name, Stop&& stop);

  void AddBus(std::string_view name, const std::list<std::string> stops, bool is_roundtrip);
  void AddBus(std::string_view name, Bus&& bus);

  Stop& GetStop(std::string_view name, std::optional<Stop> pre_node = {});
  Bus& GetBus(std::string_view name, std::optional<Bus> pre_node = {});

  const Stop& GetStop(size_t id) const;
  const Bus& GetBus(size_t id) const;

  const Stop* FindStop(std::string_view name) const;
  const Bus* FindBus(std::string_view name) const;

  const Router* GetRouter() const;
  Route FindRoute(std::string_view from, std::string_view to) const;

  std::string GetMap() const;
  std::string GetRouteMap(const Route& route) const;

 private:
  Sprav* sprav_;
  StopNames stop_names_;
  Stops stops_;
  BusNames bus_names_;
  mutable Buses buses_;

  SerializationSettings serialization_settings_;
  RoutingSettings routing_settings_;
  RenderSettings render_settings_;

  std::shared_ptr<Graph> router_graph_;
  std::shared_ptr<Router> router_;

  mutable std::shared_ptr<SpravMapper> mapper_;

  template <typename InputIt>
  void AddBusStops(size_t bus_id, InputIt begin, InputIt end);
  void BuildGraph();

  void BuildRouter();

  void CalcBusStats(Bus& r) const;

  SpravMapper& GetMapper() const;
};
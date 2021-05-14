#pragma once

#include <functional>
#include <list>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "mapper.pb.h"
#include "company.pb.h"
#include "svg.h"

class SpravMapper;
class Stop;
class Bus;

class PointProjector {
  using TaggedStops = std::unordered_map<size_t, size_t>;
  using AdjacentStops = std::unordered_map<size_t, std::unordered_set<size_t>>;

  using CoordCompressInitData = std::list<size_t>;
  using CoordCompressMap = std::unordered_map<size_t, size_t>;

  using BusLines = std::unordered_set<size_t>;
  using Companies = std::unordered_set<size_t>;
  using BearingStops = std::unordered_set<size_t>;
  using Stops = std::list<size_t>;

  struct Coords {
    double lat;
    double lon;
  };

  using StopMovedCoords = std::unordered_map<size_t, Coords>;

public:
  PointProjector(const SpravMapper& mapper);

  void PushStop(const Stop& s);
  void PushCompany(size_t id, const YellowPages::Company& c);
  void Process();
  void ParseFrom(const SpravSerialize::PointProjector& m);

  void Serialize(SpravSerialize::PointProjector& m) const;

  Svg::Point operator()(const Stop& s) const;
  Svg::Point operator()(size_t company_id) const;

private:
  const SpravMapper& mapper_;
  size_t stops_id_bound_;

  Companies companies_;
  BusLines buses_;
  StopMovedCoords moved_coords_;

  AdjacentStops adjacent_stops_;
  CoordCompressInitData x_compress_data_;
  CoordCompressMap x_compress_map_;
  CoordCompressInitData y_compress_data_;
  CoordCompressMap y_compress_map_;
  double x_step = 0;
  double y_step = 0;

  void UniformCoords();
  BearingStops FindBearingStops(const Bus& bus) const;
  void UniformCoordsForStops(const Stops& stops, size_t bearing1, size_t bearing2);

  void CompressCoords();
  AdjacentStops BuildAdjacentStops() const;
  std::pair<CoordCompressMap, size_t> CompressCoordsFor(CoordCompressInitData& data, std::function<double(size_t)> get_coord);
};

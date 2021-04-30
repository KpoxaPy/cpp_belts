#pragma once

#include <functional>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include "svg.h"

class SpravMapper;
class Stop;
class Bus;

class PointProjector {
  using CoordCompressInitData = std::list<std::unordered_set<size_t>>;
  using CoordCompressMap = std::unordered_map<size_t, size_t>;

  using BusLines = std::unordered_set<size_t>;
  using BearingStops = std::unordered_set<size_t>;

public:
  PointProjector(const SpravMapper& mapper);

  void PushStop(const Stop& s);
  void Process();

  Svg::Point operator()(const Stop& s) const;

private:
  const SpravMapper& mapper_;

  double min_lat = std::numeric_limits<double>::max();
  double max_lat = std::numeric_limits<double>::min();
  double min_lon = std::numeric_limits<double>::max();
  double max_lon = std::numeric_limits<double>::min();
  double zoom_coef = 0;

  BusLines buses_;

  CoordCompressInitData x_compress_data_;
  CoordCompressMap x_compress_map_;
  CoordCompressInitData y_compress_data_;
  CoordCompressMap y_compress_map_;
  double x_step = 0;
  double y_step = 0;

  void CalcStatsForZoom(const Stop& s);

  void UniformCoords();
  BearingStops FindBearingStops(const Bus& bus) const;

  void CompressCoords();
  bool CheckWhetherStopsAdjacent(size_t id1, size_t id2) const;
  void CompressCoordsFor(CoordCompressInitData& data, std::function<double(const Stop&)> get_coord) const;
  CoordCompressMap GetCompressMapFor(const CoordCompressInitData& data) const;
};

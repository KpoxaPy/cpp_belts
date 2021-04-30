#pragma once

#include <deque>
#include <functional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "render_settings.h"
#include "sprav.h"
#include "svg.h"

class SpravMapper;

class PointProjector {
  using CoordCompressData = std::vector<size_t>;
  using CoordCompressMap = std::unordered_map<size_t, size_t>;

public:
  PointProjector(const SpravMapper& mapper);

  void PushStop(const Stop& s);
  void Process();

  Svg::Point operator()(const Stop& s) const;

private:
  const SpravMapper& mapper_;

  double zoom_coef = 0;

  CoordCompressData x_compress_data_;
  CoordCompressMap x_compress_map_;
  CoordCompressData y_compress_data_;
  CoordCompressMap y_compress_map_;
  double x_step = 0;
  double y_step = 0;

  double min_lat = std::numeric_limits<double>::max();
  double max_lat = std::numeric_limits<double>::min();

  double min_lon = std::numeric_limits<double>::max();
  double max_lon = std::numeric_limits<double>::min();

  void CompressCoordsFor(CoordCompressData& data, std::function<double(const Stop&)> get_coord);
};

class SpravMapper {
 public:
  SpravMapper(
    const RenderSettings& settings,
    const Sprav* sprav,
    const std::deque<std::string>& stop_names,
    const std::deque<std::string>& bus_names);

  const RenderSettings& GetSettings() const;
  const std::set<std::string_view>& GetStopNames() const;
  const std::set<std::string_view>& GetBusNames() const;
  const Sprav* GetSprav() const;
  const PointProjector& GetProjector() const;

  std::string Render();

 private:
  const RenderSettings& settings_;
  const Sprav* sprav_;

  std::set<std::string_view> sorted_stop_names_;
  std::set<std::string_view> sorted_bus_names_;

  PointProjector projector_;
};
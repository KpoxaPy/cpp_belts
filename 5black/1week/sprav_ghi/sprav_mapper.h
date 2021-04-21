#pragma once

#include <set>
#include <string>
#include <string_view>
#include <deque>

#include "render_settings.h"
#include "sprav.h"
#include "svg.h"

class SpravMapper;

class PointProjector {
public:
  PointProjector(const SpravMapper& mapper);

  void PushStop(const Stop& s);

  Svg::Point operator()(const Stop& s) const;

private:
  const SpravMapper& mapper_;

  double zoom_coef = 0;

  double min_lat = std::numeric_limits<double>::max();
  double max_lat = std::numeric_limits<double>::min();

  double min_lon = std::numeric_limits<double>::max();
  double max_lon = std::numeric_limits<double>::min();
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
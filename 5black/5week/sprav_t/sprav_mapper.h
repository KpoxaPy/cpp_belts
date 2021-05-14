#pragma once

#include <deque>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <optional>

#include "mapper.pb.h"
#include "render_settings.h"
#include "sprav.h"
#include "svg.h"

class SpravMapper;
class PointProjector;

class SpravMapper {
 public:
  SpravMapper(
    const Sprav* sprav);

  SpravMapper(
    const Sprav* sprav,
    const SpravSerialize::Mapper& m);

  void Serialize(SpravSerialize::Mapper& m) const;

  const RenderSettings& GetSettings() const;
  const std::set<std::string_view>& GetStopNames() const;
  const std::set<std::string_view>& GetBusNames() const;
  const Sprav* GetSprav() const;
  const PointProjector& GetProjector() const;

  std::string Render();
  std::string RenderForRoute(const Sprav::Route& route);

 private:
  const RenderSettings& settings_;
  const Sprav* sprav_;

  std::set<std::string_view> sorted_stop_names_;
  std::set<std::string_view> sorted_bus_names_;

  std::shared_ptr<PointProjector> projector_;

  std::optional<Svg::Document> main_map_;
};
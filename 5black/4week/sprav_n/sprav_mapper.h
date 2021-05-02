#pragma once

#include <deque>
#include <memory>
#include <set>
#include <string>
#include <string_view>

#include "render_settings.h"
#include "sprav.h"
#include "svg.h"

class SpravMapper;
class PointProjector;

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

  std::shared_ptr<PointProjector> projector_;
};
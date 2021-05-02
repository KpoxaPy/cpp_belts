#include "sprav_mapper.h"

#include <sstream>
#include <algorithm>

#include "map_builder.h"
#include "point_projector.h"

using namespace std;

SpravMapper::SpravMapper(
  const RenderSettings& settings,
  const Sprav* sprav,
  const std::deque<std::string>& stop_names,
  const std::deque<std::string>& bus_names)
  : settings_(settings)
  , sprav_(sprav)
  , projector_(make_shared<PointProjector>(*this))
{
  sorted_stop_names_.insert(begin(stop_names), end(stop_names));
  sorted_bus_names_.insert(begin(bus_names), end(bus_names));

  for (const auto& name : sorted_stop_names_) {
    projector_->PushStop(*sprav_->FindStop(name));
  }
  projector_->Process();
}

const RenderSettings& SpravMapper::GetSettings() const {
  return settings_;
}

const std::set<std::string_view>& SpravMapper::GetStopNames() const {
  return sorted_stop_names_;
}

const std::set<std::string_view>& SpravMapper::GetBusNames() const {
  return sorted_bus_names_;
}

const Sprav* SpravMapper::GetSprav() const {
  return sprav_;
}

const PointProjector& SpravMapper::GetProjector() const {
  return *projector_;
}

std::string SpravMapper::Render() {
  Builder b(*this);

  for (auto layer_type : settings_.layers) {
    (b.*Builder::DRAW_ACTIONS.at(layer_type))(nullptr);
  }

  return b.Render();
}

std::string SpravMapper::RenderForRoute(const Sprav::Route& route) {
  Builder b(*this);

  for (auto layer_type : settings_.layers) {
    (b.*Builder::DRAW_ACTIONS.at(layer_type))(nullptr);
  }
  b.DrawRouterCover();
  for (auto layer_type : settings_.layers) {
    (b.*Builder::DRAW_ACTIONS.at(layer_type))(&route);
  }

  return b.Render();
}

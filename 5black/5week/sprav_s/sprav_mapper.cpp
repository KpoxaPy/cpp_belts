#include "sprav_mapper.h"

#include <sstream>
#include <algorithm>

#include "map_builder.h"
#include "point_projector.h"

using namespace std;

SpravMapper::SpravMapper(const Sprav* sprav)
  : settings_(sprav->GetRenderSettings())
  , sprav_(sprav)
{
  sorted_stop_names_.insert(begin(sprav_->GetStopNames()), end(sprav_->GetStopNames()));
  sorted_bus_names_.insert(begin(sprav_->GetBusNames()), end(sprav_->GetBusNames()));

  projector_ = make_shared<PointProjector>(*this);
  for (const auto& name : sorted_stop_names_) {
    projector_->PushStop(*sprav_->FindStop(name));
  }
  for (size_t id = 0; id < sprav_->GetPages()->Size(); ++id) {
    projector_->PushCompany(id, sprav_->GetPages()->Get(id));
  }
  projector_->Process();
}

SpravMapper::SpravMapper(const Sprav* sprav, const SpravSerialize::Mapper& m)
  : settings_(sprav->GetRenderSettings())
  , sprav_(sprav)
{
  sorted_stop_names_.insert(begin(sprav_->GetStopNames()), end(sprav_->GetStopNames()));
  sorted_bus_names_.insert(begin(sprav_->GetBusNames()), end(sprav_->GetBusNames()));

  projector_ = make_shared<PointProjector>(*this);
  projector_->ParseFrom(m.point_projector());
}

void SpravMapper::Serialize(SpravSerialize::Mapper& m) const {
  projector_->Serialize(*m.mutable_point_projector());
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

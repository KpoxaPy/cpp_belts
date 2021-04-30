#include "sprav_mapper.h"

#include <sstream>
#include <algorithm>

using namespace std;

namespace {

class Paletter {
 public:
  Paletter(const SpravMapper& mapper)
      : mapper_(mapper) {}

  const Svg::Color& operator()() const {
    const auto& palette = mapper_.GetSettings().color_palette;
    if (palette.size() == 0) {
      return Svg::NoneColor;
    }
    return palette[idx++ % palette.size()];
  }

 private:
  const SpravMapper& mapper_;
  mutable std::size_t idx = 0;
};

class Builder {
 public:
  Builder(const SpravMapper& mapper)
      : mapper_(mapper)
  {}

  void DrawBusLines() {
    Paletter paletter(mapper_);
    for (const auto& bus_name : mapper_.GetBusNames()) {
      DrawLine(paletter(), *mapper_.GetSprav()->FindBus(bus_name));
    }
  }

  void DrawStops() {
    for (const auto& stop_name : mapper_.GetStopNames()) {
      DrawStop(*mapper_.GetSprav()->FindStop(stop_name));
    }
  }

  void DrawStopNames() {
    for (const auto& stop_name : mapper_.GetStopNames()) {
      DrawStopName(*mapper_.GetSprav()->FindStop(stop_name));
    }
  }

  void DrawBusEndPoints() {
    Paletter paletter(mapper_);
    for (const auto& bus_name : mapper_.GetBusNames()) {
      const Bus& bus = *mapper_.GetSprav()->FindBus(bus_name);
      if (bus.stops.size() == 0) {
        continue;
      }

      auto line_color = paletter();
      size_t first_id = *begin(bus.stops);
      size_t last_id = *rbegin(bus.stops);
      DrawBusEndPoint(line_color, bus, mapper_.GetSprav()->GetStop(first_id));
      if (first_id != last_id) {
        DrawBusEndPoint(line_color, bus, mapper_.GetSprav()->GetStop(last_id));
      }
    }
  }

  std::string Render() {
    ostringstream ss;
    doc_.Render(ss);
    return ss.str();
  }

 public:
  static const unordered_map<MapLayerType, void (Builder::*)()> DRAW_ACTIONS;

 private:
  const SpravMapper& mapper_;
  Svg::Document doc_;

  void DrawLine(const Svg::Color& line_color, const Bus& bus) {
    Svg::Polyline line;
    line.SetStrokeColor(line_color);
    line.SetStrokeWidth(mapper_.GetSettings().line_width);
    line.SetStrokeLineCap("round");
    line.SetStrokeLineJoin("round");
    if (bus.stops.size() > 1) {
      for (const auto& stop_id : bus.stops) {
        line.AddPoint(mapper_.GetProjector()(mapper_.GetSprav()->GetStop(stop_id)));
      }
      if (!bus.is_roundtrip) {
        for (auto it = ++rbegin(bus.stops); it != rend(bus.stops); ++it) {
          line.AddPoint(mapper_.GetProjector()(mapper_.GetSprav()->GetStop(*it)));
        }
      }
    }
    doc_.Add(line);
  }

  void DrawStop(const Stop& stop) {
    doc_.Add(Svg::Circle{}
                .SetCenter(mapper_.GetProjector()(stop))
                .SetRadius(mapper_.GetSettings().stop_radius)
                .SetFillColor("white"));
  }

  void DrawStopName(const Stop& stop) {
    Svg::Text t;
    t.SetPoint(mapper_.GetProjector()(stop));
    t.SetOffset(mapper_.GetSettings().stop_label_offset);
    t.SetFontSize(mapper_.GetSettings().stop_label_font_size);
    t.SetFontFamily("Verdana");
    t.SetData(string(stop.name));
    Svg::Text underlayer = t;

    underlayer.SetFillColor(mapper_.GetSettings().underlayer_color);
    underlayer.SetStrokeColor(mapper_.GetSettings().underlayer_color);
    underlayer.SetStrokeWidth(mapper_.GetSettings().underlayer_width);
    underlayer.SetStrokeLineCap("round");
    underlayer.SetStrokeLineJoin("round");
    doc_.Add(underlayer);

    t.SetFillColor("black");
    doc_.Add(t);
  }

  void DrawBusEndPoint(const Svg::Color& line_color, const Bus& bus, const Stop& stop) {
    Svg::Text t;
    t.SetPoint(mapper_.GetProjector()(stop));
    t.SetOffset(mapper_.GetSettings().bus_label_offset);
    t.SetFontSize(mapper_.GetSettings().bus_label_font_size);
    t.SetFontFamily("Verdana");
    t.SetFontWeight("bold");
    t.SetData(string(bus.name));
    Svg::Text underlayer = t;

    underlayer.SetFillColor(mapper_.GetSettings().underlayer_color);
    underlayer.SetStrokeColor(mapper_.GetSettings().underlayer_color);
    underlayer.SetStrokeWidth(mapper_.GetSettings().underlayer_width);
    underlayer.SetStrokeLineCap("round");
    underlayer.SetStrokeLineJoin("round");
    doc_.Add(underlayer);

    t.SetFillColor(line_color);
    doc_.Add(t);
  }
};

const unordered_map<MapLayerType, void (Builder::*)()> Builder::DRAW_ACTIONS = {
  {MapLayerType::BUS_LINES, &Builder::DrawBusLines},
  {MapLayerType::BUS_LABELS, &Builder::DrawBusEndPoints},
  {MapLayerType::STOP_POINTS, &Builder::DrawStops},
  {MapLayerType::STOP_LABELS, &Builder::DrawStopNames}
};

}

PointProjector::PointProjector(const SpravMapper& mapper)
    : mapper_(mapper) {}

void PointProjector::PushStop(const Stop& s) {
  min_lat = min(s.lat, min_lat);
  max_lat = max(s.lat, max_lat);
  min_lon = min(s.lon, min_lon);
  max_lon = max(s.lon, max_lon);

  double d_lat = max_lat - min_lat;
  double d_lon = max_lon - min_lon;

  double co_width = mapper_.GetSettings().width - 2 * mapper_.GetSettings().padding;
  double co_height = mapper_.GetSettings().height - 2 * mapper_.GetSettings().padding;

  if (d_lat >= 1e-6 && d_lon >= 1e-6) {
    zoom_coef = min(co_width / d_lon, co_height / d_lat);
  } else if (d_lon >= 1e-6) {
    zoom_coef = co_width / d_lon;
  } else if (d_lat >= 1e-6) {
    zoom_coef = co_height / d_lat;
  } else {
    zoom_coef = 0;
  }

  if (mapper_.GetSettings().enableXcoordCompress) {
    x_compress_data_.push_back({s.id});
  }
  if (mapper_.GetSettings().enableYcoordCompress) {
    y_compress_data_.push_back({s.id});
  }
}

void PointProjector::Process() {
  double padding = mapper_.GetSettings().padding;
  double co_width = mapper_.GetSettings().width - 2 * padding;
  double co_height = mapper_.GetSettings().height - 2 * padding;

  if (mapper_.GetSettings().enableXcoordCompress) {
    CompressCoordsFor(x_compress_data_, [](const Stop& s){ return s.lon; });
    x_compress_map_ = GetCompressMapFor(x_compress_data_);
    x_step = co_width / max(static_cast<size_t>(1), x_compress_data_.size() - 1);
  }
  if (mapper_.GetSettings().enableYcoordCompress) {
    CompressCoordsFor(y_compress_data_, [](const Stop& s){ return s.lat; });
    y_compress_map_ = GetCompressMapFor(y_compress_data_);
    y_step = co_height / max(static_cast<size_t>(1), y_compress_data_.size() - 1);
  }
}

Svg::Point PointProjector::operator()(const Stop& s) const {
  double padding = mapper_.GetSettings().padding;

  double x, y;
  if (mapper_.GetSettings().enableXcoordCompress) {
    x = x_compress_map_.at(s.id) * x_step + padding;
  } else {
    x = (s.lon - min_lon) * zoom_coef + padding;
  }

  if (mapper_.GetSettings().enableYcoordCompress) {
    y = mapper_.GetSettings().height - padding - y_compress_map_.at(s.id) * y_step;
  } else {
    y = (max_lat - s.lat) * zoom_coef + padding;
  }

  return {x, y};
}

bool PointProjector::CheckWhetherStopsAdjacent(size_t id1, size_t id2) {
  if (id1 == id2) {
    return false;
  }

  const Sprav* sprav = mapper_.GetSprav();
  const Stop& stop1 = sprav->GetStop(id1);
  const Stop& stop2 = sprav->GetStop(id2);

  std::unordered_set<size_t> shared_buses;
  set_intersection(stop1.buses.begin(), stop1.buses.end(),
    stop2.buses.begin(), stop2.buses.end(),
    inserter(shared_buses, shared_buses.end()));

  if (shared_buses.size() == 0) {
    return false;
  }

  for (auto bus_id : shared_buses) {
    const Bus& bus = sprav->GetBus(bus_id);
    auto it = find(bus.stops.begin(), bus.stops.end(), id1);

    if (it != bus.stops.begin()) {
      if (*prev(it) == id2) {
        return true;
      }
    } else if (bus.is_roundtrip && *prev(prev(bus.stops.end())) == id2) {
      return true;
    }

    if (it != prev(bus.stops.end()) && *next(it) == id2) {
      return true;
    }
  }
  return false;
}

void PointProjector::CompressCoordsFor(CoordCompressInitData& data, std::function<double(const Stop&)> get_coord) {
  data.sort([this, &get_coord](unordered_set<size_t> lhs, unordered_set<size_t> rhs) {
    return get_coord(mapper_.GetSprav()->GetStop(*lhs.begin()))
      < get_coord(mapper_.GetSprav()->GetStop(*rhs.begin()));
  });

  if (data.size() <= 1) {
    return;
  }
  
  for (auto it = next(data.begin()); it != data.end();) {
    auto previous_it = prev(it);
    auto current_stop_id = *it->begin();
    bool has_adjacent_stop = false;

    for (auto id : *previous_it) {
      if (CheckWhetherStopsAdjacent(current_stop_id, id)) {
        has_adjacent_stop = true;
        break;
      }
    }

    if (!has_adjacent_stop) {
      for (auto id : *it) {
        previous_it->insert(id);
      }
      it = data.erase(it);
    } else {
      ++it;
    }
  }
}

PointProjector::CoordCompressMap PointProjector::GetCompressMapFor(const CoordCompressInitData& data) {
  CoordCompressMap map;
  size_t i = 0;
  for (auto it = data.begin(); it != data.end(); ++i, ++it) {
    for (size_t id : *it) {
      map[id] = i;
    }
  }
  return map;
}

SpravMapper::SpravMapper(
  const RenderSettings& settings,
  const Sprav* sprav,
  const std::deque<std::string>& stop_names,
  const std::deque<std::string>& bus_names)
  : settings_(settings)
  , sprav_(sprav)
  , projector_(*this)
{
  sorted_stop_names_.insert(begin(stop_names), end(stop_names));
  sorted_bus_names_.insert(begin(bus_names), end(bus_names));

  for (const auto& name : sorted_stop_names_) {
    projector_.PushStop(*sprav_->FindStop(name));
  }
  projector_.Process();
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
  return projector_;
}

std::string SpravMapper::Render() {
  Builder b(*this);

  for (auto layer_type : settings_.layers) {
    (b.*Builder::DRAW_ACTIONS.at(layer_type))();
  }

  return b.Render();
}

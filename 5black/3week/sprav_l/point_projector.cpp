#include "point_projector.h"

#include "sprav_mapper.h"

using namespace std;

PointProjector::PointProjector(const SpravMapper& mapper)
    : mapper_(mapper) {}

void PointProjector::PushStop(const Stop& s) {
  CalcStatsForZoom(s);

  for (auto bus_id : s.buses) {
    buses_.insert(bus_id);
  }

  if (mapper_.GetSettings().enableXcoordCompress) {
    x_compress_data_.push_back({s.id});
  }
  if (mapper_.GetSettings().enableYcoordCompress) {
    y_compress_data_.push_back({s.id});
  }
}

void PointProjector::Process() {
  UniformCoords();
  CompressCoords();
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

void PointProjector::CalcStatsForZoom(const Stop& s) {
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
}

void PointProjector::UniformCoords() {
  for (auto bus_id : buses_) {
    const Bus& bus = mapper_.GetSprav()->GetBus(bus_id);
    auto bearing_stops_ = FindBearingStops(bus);

    UNUSED(bearing_stops_);
  }
}

PointProjector::BearingStops PointProjector::FindBearingStops(const Bus& bus) const {
  BearingStops stops;

  unordered_map<size_t, size_t> stops_rate;

  for (auto it = bus.stops.begin(); it != bus.stops.end(); ++it) {
    const Stop& stop = mapper_.GetSprav()->GetStop(*it);

    auto rate_it = stops_rate.find(stop.id);
    if (rate_it == stops_rate.end()) {
      rate_it = stops_rate.emplace(stop.id, 0).first;
    }

    rate_it->second += 1;

    if (it == bus.stops.begin()) {
      rate_it->second += 2;
    }

    if (stop.buses.size() > 1) {
      rate_it->second += 2;
    }
  }

  if (!bus.is_roundtrip) {
    for (auto it = bus.stops.rbegin(); it != bus.stops.rend(); ++it) {
      const Stop& stop = mapper_.GetSprav()->GetStop(*it);
      auto rate_it = stops_rate.find(stop.id);

      rate_it->second += 1;

      if (it == bus.stops.rbegin()) {
        rate_it->second += 2;
      }
    }
  }

  for (auto [stop_id, rate] : stops_rate) {
    if (rate > 2) {
      stops.insert(stop_id);
    }
  }

  return stops;
}

void PointProjector::CompressCoords() {
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

bool PointProjector::CheckWhetherStopsAdjacent(size_t id1, size_t id2) const {
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

    auto it = bus.stops.begin();
    while (true) {
      it = find(it, bus.stops.end(), id1);
      if (it == bus.stops.end()) {
        break;
      }

      if (it == bus.stops.begin() && bus.is_roundtrip && *prev(prev(bus.stops.end())) == id2) {
        return true;
      } else if (*prev(it) == id2) {
        return true;
      }

      if (it != prev(bus.stops.end()) && *next(it) == id2) {
        return true;
      }

      ++it;
    }

  }
  return false;
}

void PointProjector::CompressCoordsFor(CoordCompressInitData& data, std::function<double(const Stop&)> get_coord) const {
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

PointProjector::CoordCompressMap PointProjector::GetCompressMapFor(const CoordCompressInitData& data) const {
  CoordCompressMap map;
  size_t i = 0;
  for (auto it = data.begin(); it != data.end(); ++i, ++it) {
    for (size_t id : *it) {
      map[id] = i;
    }
  }
  return map;
}
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
  moved_coords_[s.id] = {s.lat, s.lon};

  if (mapper_.GetSettings().enableXcoordCompress) {
    x_compress_data_.push_back(s.id);
  }
  if (mapper_.GetSettings().enableYcoordCompress) {
    y_compress_data_.push_back(s.id);
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

    Stops stops;
    size_t bearing_id = 0;
    for (auto it = bus.stops.begin(); it != bus.stops.end(); ++it) {
      if (bearing_stops_.count(*it) > 0) {
        if (stops.size() > 0) {
          UniformCoordsForStops(stops, bearing_id, *it);
          stops.clear();
        }
        bearing_id = *it;
      } else {
        stops.push_back(*it);
      }
    }
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

void PointProjector::UniformCoordsForStops(const Stops& stops, size_t bearing1, size_t bearing2) {
  double lat_begin = moved_coords_[bearing1].lat;
  double lon_begin = moved_coords_[bearing1].lon;
  double lat_step = (moved_coords_[bearing2].lat - lat_begin) / (stops.size() + 1);
  double lon_step = (moved_coords_[bearing2].lon - lon_begin) / (stops.size() + 1);
  
  size_t i = 1;
  for (auto it = stops.begin(); it != stops.end(); ++i, ++it) {
    auto& c = moved_coords_[*it];
    c.lat = lat_begin + lat_step * i;
    c.lon = lon_begin + lon_step * i;
  }
}

void PointProjector::CompressCoords() {
  adjacent_stops_ = BuildAdjacentStops();

  double padding = mapper_.GetSettings().padding;
  double co_width = mapper_.GetSettings().width - 2 * padding;
  double co_height = mapper_.GetSettings().height - 2 * padding;

  size_t tags_count;
  if (mapper_.GetSettings().enableXcoordCompress) {
    tie(x_compress_map_, tags_count) = CompressCoordsFor(
      x_compress_data_, [this](size_t id){ return moved_coords_[id].lon; });
    x_step = co_width / max(static_cast<size_t>(1), tags_count - 1);
  }

  if (mapper_.GetSettings().enableYcoordCompress) {
    tie(y_compress_map_, tags_count) = CompressCoordsFor(
      y_compress_data_, [this](size_t id){ return moved_coords_[id].lat; });
    y_step = co_height / max(static_cast<size_t>(1), tags_count - 1);
  }
}

PointProjector::AdjacentStops PointProjector::BuildAdjacentStops() const {
  AdjacentStops map;

  for (auto bus_id : buses_) {
    const Bus& bus = mapper_.GetSprav()->GetBus(bus_id);

    if (bus.stops.size() <= 1) {
      continue;
    }

    for (auto it = bus.stops.begin(); it != bus.stops.end(); ++it) {
      if (it != bus.stops.begin()) {
        map[*it].insert(*prev(it));
      }
      if (it != prev(bus.stops.end())) {
        map[*it].insert(*next(it));
      }
    }
  }

  return map;
}

pair<PointProjector::CoordCompressMap, size_t>
PointProjector::CompressCoordsFor(CoordCompressInitData& data, std::function<double(size_t)> get_coord) {
  data.sort([this, &get_coord](size_t lhs, size_t rhs) {
    return get_coord(lhs) < get_coord(rhs);
  });

  TaggedStops tags;
  
  for (auto stop_id : data) {
    size_t tag = 0;
    bool has_tagged_adjacent = false;

    for (auto adjacent_stop_id : adjacent_stops_[stop_id]) {
      if (get_coord(adjacent_stop_id) < get_coord(stop_id)) {
        if (auto it = tags.find(adjacent_stop_id); it != tags.end()) {
          tag = max(tag, it->second);
          has_tagged_adjacent = true;
        }
      }
    }

    if (has_tagged_adjacent) {
      ++tag;
    }

    tags[stop_id] = tag;
  }

  CoordCompressMap map;
  unordered_set<size_t> tags_set;

  for (auto [stop_id, tag] : tags) {
    map[stop_id] = tag;
    tags_set.insert(tag);
  }

  return {move(map), tags_set.size()};
}

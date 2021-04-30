#include "point_projector.h"

#include "sprav_mapper.h"

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
    x_compress_data_.push_back(s.id);
  }
  if (mapper_.GetSettings().enableYcoordCompress) {
    y_compress_data_.push_back(s.id);
  }
}

void PointProjector::Process() {
  double padding = mapper_.GetSettings().padding;
  double co_width = mapper_.GetSettings().width - 2 * padding;
  double co_height = mapper_.GetSettings().height - 2 * padding;

  if (mapper_.GetSettings().enableXcoordCompress) {
    CompressCoordsFor(x_compress_data_, [](const Stop& s){ return s.lon; });
    for (size_t i = 0; i < x_compress_data_.size(); ++i) {
      x_compress_map_[x_compress_data_[i]] = i;
    }
    x_step = co_width / max(static_cast<size_t>(1), x_compress_data_.size() - 1);
  }
  if (mapper_.GetSettings().enableYcoordCompress) {
    CompressCoordsFor(y_compress_data_, [](const Stop& s){ return s.lat; });
    for (size_t i = 0; i < y_compress_data_.size(); ++i) {
      y_compress_map_[y_compress_data_[i]] = i;
    }
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

void PointProjector::CompressCoordsFor(CoordCompressData& data, std::function<double(const Stop&)> get_coord) {
  sort(data.begin(), data.end(), [this, &get_coord](size_t lhs, size_t rhs) {
    return get_coord(mapper_.GetSprav()->GetStop(lhs))
      < get_coord(mapper_.GetSprav()->GetStop(rhs));
  });
}

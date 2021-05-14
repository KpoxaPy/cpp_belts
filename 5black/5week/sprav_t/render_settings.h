#pragma once

#include <vector>

#include "json.h"
#include "svg.h"
#include "render_settings.pb.h"

enum class MapLayerType {
  BUS_LINES,
  BUS_LABELS,
  STOP_POINTS,
  STOP_LABELS,
  COMPANY_LINES,
  COMPANY_POINTS,
  COMPANY_LABELS
};

struct RenderSettings {
  RenderSettings() = default;
  RenderSettings(const Json::Dict& dict);
  RenderSettings(const SpravSerialize::RenderSettings& m);

  void Serialize(SpravSerialize::RenderSettings& m) const;

  double width = 1000;
  double height = 1000;
  double padding = 10;
  double outer_margin = 0;

  double stop_radius = 6;
  double line_width = 16;

  double company_radius = 5;
  double company_line_width = 8;

  uint32_t stop_label_font_size = 1;
  Svg::Point stop_label_offset = {1, 1};

  Svg::Color underlayer_color;
  double underlayer_width = 1;

  double bus_label_font_size = 1;
  Svg::Point bus_label_offset = {-1, 1};

  std::vector<Svg::Color> color_palette;

  std::vector<MapLayerType> layers;
};
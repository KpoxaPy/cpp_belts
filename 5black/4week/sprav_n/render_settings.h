#pragma once

#include <vector>

#include "json.h"
#include "svg.h"
#include "map_builder.h"

struct RenderSettings {
  RenderSettings() = default;
  RenderSettings(const Json::Map& dict);

  double width = 1000;
  double height = 1000;
  double padding = 10;

  double stop_radius = 6;
  double line_width = 16;

  uint32_t stop_label_font_size = 1;
  Svg::Point stop_label_offset = {1, 1};

  Svg::Color underlayer_color;
  double underlayer_width = 1;

  double bus_label_font_size = 1;
  Svg::Point bus_label_offset = {-1, 1};

  std::vector<Svg::Color> color_palette;

  std::vector<MapLayerType> layers;

  bool enableXcoordCompress = true;
  bool enableYcoordCompress = true;
};
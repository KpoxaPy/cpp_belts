#include "render_settings.h"

#include <stdexcept>

using namespace std;

namespace {

Svg::Color ColorFromJson(const Json::Node& node) {
  if (holds_alternative<string>(node)) {
    return node.AsString();  
  } else if (holds_alternative<Json::Array>(node)) {
    const auto& arr = node.AsArray();
    if (arr.size() == 3 || arr.size() == 4) {
      uint8_t r = arr[0].AsNumber();
      uint8_t g = arr[1].AsNumber();
      uint8_t b = arr[2].AsNumber();
      if (arr.size() == 3) {
        return Svg::Rgb{r, g, b};
      } else if (arr.size() == 4) {
        double a = arr[3].AsNumber();
        return Svg::Rgb{r, g, b, a};
      }
    }
  }
  ostringstream ss;
  ss << "wrong color format in json node: " << node;
  throw runtime_error(ss.str());
}

Svg::Point PointFromJson(const Json::Node& node) {
  const auto& array = node.AsArray();
  return {array[0].AsNumber(), array[1].AsNumber()};
}

}

RenderSettings::RenderSettings(const Json::Map& dict) {
  width = dict.at("width").AsNumber();
  height = dict.at("height").AsNumber();
  padding = dict.at("padding").AsNumber();

  stop_radius = dict.at("stop_radius").AsNumber();
  line_width = dict.at("line_width").AsNumber();

  underlayer_color = ColorFromJson(dict.at("underlayer_color"));
  underlayer_width = dict.at("underlayer_width").AsNumber();

  stop_label_font_size = dict.at("stop_label_font_size").AsNumber();
  stop_label_offset = PointFromJson(dict.at("stop_label_offset"));

  bus_label_font_size = dict.at("bus_label_font_size").AsNumber();
  bus_label_offset = PointFromJson(dict.at("bus_label_offset"));

  const auto& palette_array = dict.at("color_palette").AsArray();
  color_palette.reserve(palette_array.size());
  for (const auto& c : palette_array) {
    color_palette.push_back(ColorFromJson(c));
  }

  const auto& layers_array = dict.at("layers").AsArray();
  layers.reserve(layers_array.size());
  for (const auto& l : layers_array) {
    MapLayerType t;
    const auto& s = l.AsString();
    if (s == "bus_lines") {
      t = MapLayerType::BUS_LINES;
    } else if (s == "bus_labels") {
      t = MapLayerType::BUS_LABELS;
    } else if (s == "stop_points") {
      t = MapLayerType::STOP_POINTS;
    } else if (s == "stop_labels") {
      t = MapLayerType::STOP_LABELS;
    }
    layers.push_back(t);
  }
}
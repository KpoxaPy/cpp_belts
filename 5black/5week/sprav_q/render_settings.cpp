#include "render_settings.h"

#include <stdexcept>

using namespace std;

namespace {

Svg::Color ColorFromJson(const Json::Node& node) {
  if (holds_alternative<string>(node.GetBase())) {
    return node.AsString();  
  } else if (holds_alternative<Json::Array>(node.GetBase())) {
    const auto& arr = node.AsArray();
    if (arr.size() == 3 || arr.size() == 4) {
      uint8_t r = arr[0].AsInt();
      uint8_t g = arr[1].AsInt();
      uint8_t b = arr[2].AsInt();
      if (arr.size() == 3) {
        return Svg::Rgb{r, g, b};
      } else if (arr.size() == 4) {
        double a = arr[3].AsDouble();
        return Svg::Rgb{r, g, b, a};
      }
    }
  }
  ostringstream ss;
  ss << "wrong color format in json node: ";
  Json::PrintNode(node, ss);
  throw runtime_error(ss.str());
}

Svg::Point PointFromJson(const Json::Node& node) {
  const auto& array = node.AsArray();
  return {array[0].AsDouble(), array[1].AsDouble()};
}

}

RenderSettings::RenderSettings(const Json::Dict& dict) {
  width = dict.at("width").AsInt();
  height = dict.at("height").AsInt();
  padding = dict.at("padding").AsInt();
  outer_margin = dict.at("outer_margin").AsInt();

  stop_radius = dict.at("stop_radius").AsInt();
  line_width = dict.at("line_width").AsInt();

  underlayer_color = ColorFromJson(dict.at("underlayer_color"));
  underlayer_width = dict.at("underlayer_width").AsInt();

  stop_label_font_size = dict.at("stop_label_font_size").AsInt();
  stop_label_offset = PointFromJson(dict.at("stop_label_offset"));

  bus_label_font_size = dict.at("bus_label_font_size").AsInt();
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

RenderSettings::RenderSettings(const SpravSerialize::RenderSettings& m) {
  width = m.width();

  height = m.height();
  padding = m.padding();
  outer_margin = m.outer_margin();

  stop_radius = m.stop_radius();
  line_width = m.line_width();

  underlayer_color = Svg::Color(m.underlayer_color());
  underlayer_width = m.underlayer_width();

  stop_label_font_size = m.stop_label_font_size();
  stop_label_offset = Svg::Point(m.stop_label_offset());

  bus_label_font_size = m.bus_label_font_size();
  bus_label_offset = Svg::Point(m.bus_label_offset());

  color_palette.reserve(m.color_palette().size());
  for (const auto& c : m.color_palette()) {
    color_palette.emplace_back(c);
  }

  layers.reserve(m.layers().size());
  for (const auto& l : m.layers()) {
    MapLayerType t;
    switch (l)
    {
    case SpravSerialize::MapLayerType::BUS_LINES:
      t = MapLayerType::BUS_LINES; break;
    case SpravSerialize::MapLayerType::BUS_LABELS:
      t = MapLayerType::BUS_LABELS; break;
    case SpravSerialize::MapLayerType::STOP_LABELS:
      t = MapLayerType::STOP_LABELS; break;
    case SpravSerialize::MapLayerType::STOP_POINTS:
      t = MapLayerType::STOP_POINTS; break;
    
    case SpravSerialize::MapLayerType::UNKNOWN:
    default:
      throw runtime_error("Unknown map layer type");
    }
    layers.push_back(t);
  }
}

void RenderSettings::Serialize(SpravSerialize::RenderSettings& m) const {
  m.set_width(width);

  m.set_height(height);
  m.set_padding(padding);
  m.set_outer_margin(outer_margin);

  m.set_stop_radius(stop_radius);
  m.set_line_width(line_width);

  underlayer_color.Serialize(*m.mutable_underlayer_color());
  m.set_underlayer_width(underlayer_width);

  m.set_stop_label_font_size(stop_label_font_size);
  stop_label_offset.Serialize(*m.mutable_stop_label_offset());

  m.set_bus_label_font_size(bus_label_font_size);
  bus_label_offset.Serialize(*m.mutable_bus_label_offset());

  for (const auto& c : color_palette) {
    c.Serialize(*m.add_color_palette());
  }

  for (const auto& l : layers) {
    SpravSerialize::MapLayerType t;
    switch (l)
    {
    case MapLayerType::BUS_LABELS:
      t = SpravSerialize::MapLayerType::BUS_LABELS; break;
    case MapLayerType::BUS_LINES:
      t = SpravSerialize::MapLayerType::BUS_LINES; break;
    case MapLayerType::STOP_LABELS:
      t = SpravSerialize::MapLayerType::STOP_LABELS; break;
    case MapLayerType::STOP_POINTS:
      t = SpravSerialize::MapLayerType::STOP_POINTS; break;
    
    default:
      t = SpravSerialize::MapLayerType::UNKNOWN;
      break;
    }
    m.add_layers(t);
  }
}
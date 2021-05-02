#include "map_builder.h"

#include "paletter.h"
#include "sprav_mapper.h"
#include "point_projector.h"

const unordered_map<MapLayerType, void (Builder::*)()> Builder::DRAW_ACTIONS = {
    {MapLayerType::BUS_LINES, &Builder::DrawBusLines},
    {MapLayerType::BUS_LABELS, &Builder::DrawBusEndPoints},
    {MapLayerType::STOP_POINTS, &Builder::DrawStops},
    {MapLayerType::STOP_LABELS, &Builder::DrawStopNames}};

Builder::Builder(const SpravMapper& mapper)
  : mapper_(mapper)
{}

void Builder::DrawBusLines() {
  Paletter paletter(mapper_);
  for (const auto& bus_name : mapper_.GetBusNames()) {
    DrawLine(paletter(), *mapper_.GetSprav()->FindBus(bus_name));
  }
}

void Builder::DrawStops() {
  for (const auto& stop_name : mapper_.GetStopNames()) {
    DrawStop(*mapper_.GetSprav()->FindStop(stop_name));
  }
}

void Builder::DrawStopNames() {
  for (const auto& stop_name : mapper_.GetStopNames()) {
    DrawStopName(*mapper_.GetSprav()->FindStop(stop_name));
  }
}

void Builder::DrawBusEndPoints() {
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

std::string Builder::Render() {
  ostringstream ss;
  doc_.Render(ss);
  return ss.str();
}

void Builder::DrawLine(const Svg::Color& line_color, const Bus& bus) {
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

void Builder::DrawStop(const Stop& stop) {
  doc_.Add(Svg::Circle{}
               .SetCenter(mapper_.GetProjector()(stop))
               .SetRadius(mapper_.GetSettings().stop_radius)
               .SetFillColor("white"));
}

void Builder::DrawStopName(const Stop& stop) {
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

void Builder::DrawBusEndPoint(const Svg::Color& line_color, const Bus& bus, const Stop& stop) {
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
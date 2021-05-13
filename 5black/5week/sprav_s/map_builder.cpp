#include "map_builder.h"

#include "paletter.h"
#include "sprav_mapper.h"
#include "point_projector.h"

const unordered_map<MapLayerType, void (Builder::*)(const Sprav::Route*)> Builder::DRAW_ACTIONS = {
    {MapLayerType::BUS_LINES, &Builder::DrawBusLines},
    {MapLayerType::BUS_LABELS, &Builder::DrawBusEndPoints},
    {MapLayerType::STOP_POINTS, &Builder::DrawStops},
    {MapLayerType::STOP_LABELS, &Builder::DrawStopNames},
    {MapLayerType::COMPANY_LINES, &Builder::DrawCompanyLines},
    {MapLayerType::COMPANY_POINTS, &Builder::DrawCompanyPoints},
    {MapLayerType::COMPANY_LABELS, &Builder::DrawCompanyLabels}};

Builder::Builder(const SpravMapper& mapper)
  : mapper_(mapper)
{
  BuildBusLinesPalette();
}

void Builder::DrawBusLines(const Sprav::Route* route) {
  if (route) {
    for (auto part : *route) {
      if (part.type == RoutePartType::RIDE_BUS) {
        DrawLine(bus_lines_palette_[part.name], part.stops);
      }
    }
  } else {
    for (const auto& bus_name : mapper_.GetBusNames()) {
      const Bus& bus = *mapper_.GetSprav()->FindBus(bus_name);
      DrawLineFull(bus_lines_palette_[bus_name], bus);
    }
  }
}

void Builder::DrawStops(const Sprav::Route* route) {
  if (route) {
    for (auto part : *route) {
      if (part.type == RoutePartType::RIDE_BUS) {
        for (auto stop_id : part.stops) {
          DrawStop(mapper_.GetSprav()->GetStop(stop_id));
        }
      }
    }
  } else {
    for (const auto& stop_name : mapper_.GetStopNames()) {
      DrawStop(*mapper_.GetSprav()->FindStop(stop_name));
    }
  }
}

void Builder::DrawStopNames(const Sprav::Route* route) {
  if (route) {
    std::optional<size_t> last_stop_id;
    bool only_walk = true;
    for (auto part : *route) {
      if (part.type == RoutePartType::WAIT_BUS) {
        DrawStopName(*mapper_.GetSprav()->FindStop(part.name));
        only_walk = false;
      } else if (part.type == RoutePartType::RIDE_BUS) {
        last_stop_id = *part.stops.rbegin();
        only_walk = false;
      } else if (part.type == RoutePartType::WALK_TO_COMPANY && only_walk) {
        DrawStopName(*mapper_.GetSprav()->FindStop(part.name));
      }
    }
    if (last_stop_id) {
      DrawStopName(mapper_.GetSprav()->GetStop(*last_stop_id));
    }
  } else {
    for (const auto& stop_name : mapper_.GetStopNames()) {
      DrawStopName(*mapper_.GetSprav()->FindStop(stop_name));
    }
  }
}

void Builder::DrawBusEndPoints(const Sprav::Route* route) {
  if (route) {
    for (auto part : *route) {
      if (part.type == RoutePartType::RIDE_BUS) {
        const Bus& bus = *mapper_.GetSprav()->FindBus(part.name);
        size_t first_end_stop_id = *begin(bus.stops);
        size_t last_end_stop_id = *rbegin(bus.stops);

        size_t first_id = *part.stops.begin();
        size_t last_id = *part.stops.rbegin();
        if (first_id == first_end_stop_id || first_id == last_end_stop_id) {
          DrawBusEndPoint(bus_lines_palette_[part.name], bus, mapper_.GetSprav()->GetStop(first_id));
        }
        if (last_id == first_end_stop_id || last_id == last_end_stop_id) {
          DrawBusEndPoint(bus_lines_palette_[part.name], bus, mapper_.GetSprav()->GetStop(last_id));
        }
      }
    }
  } else {
    for (const auto& bus_name : mapper_.GetBusNames()) {
      const Bus& bus = *mapper_.GetSprav()->FindBus(bus_name);
      if (bus.stops.size() == 0) {
        continue;
      }

      size_t first_id = *begin(bus.stops);
      size_t last_id = *rbegin(bus.stops);
      DrawBusEndPoint(bus_lines_palette_[bus_name], bus, mapper_.GetSprav()->GetStop(first_id));
      if (first_id != last_id) {
        DrawBusEndPoint(bus_lines_palette_[bus_name], bus, mapper_.GetSprav()->GetStop(last_id));
      }
    }
  }
}

void Builder::DrawCompanyLines(const Sprav::Route* route) {
  if (!route) {
    return;
  }

  for (auto part : *route) {
    if (part.type == RoutePartType::WALK_TO_COMPANY) {
      const auto& stop = *mapper_.GetSprav()->FindStop(part.name);
      Svg::Polyline line;
      line.SetStrokeColor("black");
      line.SetStrokeWidth(mapper_.GetSettings().company_line_width);
      line.SetStrokeLineCap("round");
      line.SetStrokeLineJoin("round");
      line.AddPoint(mapper_.GetProjector()(stop));
      line.AddPoint(mapper_.GetProjector()(part.company_id));
      doc_.Add(line);
    }
  }
}

void Builder::DrawCompanyPoints(const Sprav::Route* route) {
  if (!route) {
    return;
  }

  for (auto part : *route) {
    if (part.type == RoutePartType::WALK_TO_COMPANY) {
      doc_.Add(Svg::Circle{}
                  .SetCenter(mapper_.GetProjector()(part.company_id))
                  .SetRadius(mapper_.GetSettings().company_radius)
                  .SetFillColor("black"));
    }
  }
}

void Builder::DrawCompanyLabels(const Sprav::Route* route) {
  if (!route) {
    return;
  }

  for (auto part : *route) {
    if (part.type == RoutePartType::WALK_TO_COMPANY) {
      string name = mapper_.GetSprav()->GetPages()->GetCompanyFullName(part.company_id);
      DrawStopName(mapper_.GetProjector()(part.company_id), std::move(name));
    }
  }
}

void Builder::DrawRouterCover() {
  Svg::Rect rect;
  
  double outer_margin = mapper_.GetSettings().outer_margin;
  double width = mapper_.GetSettings().width;
  double height = mapper_.GetSettings().height;
  rect.SetStart({-outer_margin, -outer_margin});
  rect.SetDimensions({width + outer_margin * 2, height + outer_margin * 2});
  rect.SetFillColor(mapper_.GetSettings().underlayer_color);

  doc_.Add(rect);
}

std::string Builder::Render() {
  ostringstream ss;
  doc_.Render(ss);
  return ss.str();
}

void Builder::BuildBusLinesPalette() {
  Paletter paletter(mapper_);
  for (const auto& bus_name : mapper_.GetBusNames()) {
    bus_lines_palette_[bus_name] = paletter();
  }
}

void Builder::DrawLineFull(const Svg::Color& line_color, const Bus& bus) {
  std::list<size_t> stops;
  if (bus.stops.size() > 1) {
    for (const auto& stop_id : bus.stops) {
      stops.push_back(stop_id);
    }
    if (!bus.is_roundtrip) {
      for (auto it = ++rbegin(bus.stops); it != rend(bus.stops); ++it) {
        stops.push_back(*it);
      }
    }
  }
  DrawLine(line_color, stops);
}

void Builder::DrawLine(const Svg::Color& line_color, const std::list<size_t>& stops) {
  Svg::Polyline line;
  line.SetStrokeColor(line_color);
  line.SetStrokeWidth(mapper_.GetSettings().line_width);
  line.SetStrokeLineCap("round");
  line.SetStrokeLineJoin("round");
  for (auto id : stops) {
    line.AddPoint(mapper_.GetProjector()(mapper_.GetSprav()->GetStop(id)));
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
  Builder::DrawStopName(mapper_.GetProjector()(stop), string(stop.name));
}

void Builder::DrawStopName(Svg::Point point, std::string name) {
  Svg::Text t;
  t.SetPoint(std::move(point));
  t.SetOffset(mapper_.GetSettings().stop_label_offset);
  t.SetFontSize(mapper_.GetSettings().stop_label_font_size);
  t.SetFontFamily("Verdana");
  t.SetData(std::move(name));
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
#pragma once

#include <unordered_map>

#include "svg.h"
#include "sprav.h"
#include "render_settings.h"

class SpravMapper;
class Stop;
class Bus;

class Builder {
 public:
  Builder(const SpravMapper& mapper);

  void DrawBusLines(const Sprav::Route* route = nullptr);
  void DrawStops(const Sprav::Route* route = nullptr);
  void DrawStopNames(const Sprav::Route* route = nullptr);
  void DrawBusEndPoints(const Sprav::Route* route = nullptr);
  void DrawCompanyLines(const Sprav::Route* route = nullptr);
  void DrawCompanyPoints(const Sprav::Route* route = nullptr);
  void DrawCompanyLabels(const Sprav::Route* route = nullptr);

  void DrawRouterCover();

  std::string Render();

 public:
  static const std::unordered_map<MapLayerType, void (Builder::*)(const Sprav::Route*)> DRAW_ACTIONS;

 private:
  const SpravMapper& mapper_;
  Svg::Document doc_;
  std::unordered_map<std::string_view, Svg::Color> bus_lines_palette_;

  void BuildBusLinesPalette();

  void DrawLineFull(const Svg::Color& line_color, const Bus& bus);
  void DrawLine(const Svg::Color& line_color, const std::list<size_t>& stops);

  void DrawStop(const Stop& stop);
  void DrawStopName(const Stop& stop);
  void DrawStopName(Svg::Point point, std::string name);
  void DrawBusEndPoint(const Svg::Color& line_color, const Bus& bus, const Stop& stop);
};
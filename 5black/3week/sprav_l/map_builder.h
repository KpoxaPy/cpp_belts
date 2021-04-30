#pragma once

#include <unordered_map>

#include "svg.h"

enum class MapLayerType {
  BUS_LINES,
  BUS_LABELS,
  STOP_POINTS,
  STOP_LABELS
};

class SpravMapper;
class Stop;
class Bus;

class Builder {
 public:
  Builder(const SpravMapper& mapper);

  void DrawBusLines();
  void DrawStops();
  void DrawStopNames();
  void DrawBusEndPoints();

  std::string Render();

 public:
  static const std::unordered_map<MapLayerType, void (Builder::*)()> DRAW_ACTIONS;

 private:
  const SpravMapper& mapper_;
  Svg::Document doc_;

  void DrawLine(const Svg::Color& line_color, const Bus& bus);
  void DrawStop(const Stop& stop);
  void DrawStopName(const Stop& stop);
  void DrawBusEndPoint(const Svg::Color& line_color, const Bus& bus, const Stop& stop);
};
#pragma once

#include <string>

#include "json.h"

struct SerializationSettings {
  SerializationSettings() = default;
  SerializationSettings(const Json::Map& dict);

  std::string file;
};
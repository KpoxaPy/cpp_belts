#pragma once

#include <string>

#include "json.h"

struct SerializationSettings {
  SerializationSettings() = default;
  SerializationSettings(const Json::Dict& dict);

  std::string file;
};
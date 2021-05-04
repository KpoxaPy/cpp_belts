#include "serialization_settings.h"

SerializationSettings::SerializationSettings(const Json::Map& dict) {
  file = dict.at("file").AsString();
}

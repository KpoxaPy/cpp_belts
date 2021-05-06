#include "serialization_settings.h"

SerializationSettings::SerializationSettings(const Json::Dict& dict) {
  file = dict.at("file").AsString();
}

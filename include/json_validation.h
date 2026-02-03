#pragma once

#include <ArduinoJson.h>

bool parseJson(byte *payload, unsigned int length, JsonDocument &doc);
#pragma once

#include <ArduinoJson.h>

bool parseJson(byte *payload, unsigned int length, StaticJsonDocument<128> &doc);
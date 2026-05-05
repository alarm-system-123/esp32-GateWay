#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "topics.h"
#include "json_validation.h"
#include "commands.h"
void mqttCallback(char *topic, byte *payload, unsigned int length);

#pragma once
#include <ArduinoJson.h>

#include "device_controller.h"
#include "commandentry.h"

void handleCommand(const JsonDocument &doc);
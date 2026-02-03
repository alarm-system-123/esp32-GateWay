#pragma once
#include <ArduinoJson.h>

void handleCommand(const JsonDocument &doc);
void commands(const JsonDocument &doc);
void sensors(const JsonDocument &doc);
#pragma once
#include "mqtt_manager.h"
#include <ArduinoJson.h>
#include "structure_sensor.h"

void armSystem(const JsonDocument &doc);
void disarmSystem(const JsonDocument &doc);
void statusSystem(const JsonDocument &doc);
void sensorStatus();
void armPartial(const JsonDocument &doc);
void armGroup(const JsonDocument &doc);
void addSensor(const JsonDocument &doc);
void removeSensor(const JsonDocument &doc);
void updateSensorConfig(const JsonDocument &doc);
void publishSingleSensor(SensorNode *node);
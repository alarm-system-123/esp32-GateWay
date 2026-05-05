#pragma once
#include <Arduino.h>
#include <WiFi.h>

// Need for MQTT connection
extern String TOPIC_COMMANDS;
extern String TOPIC_GATEWAY_STATUS;
extern String TOPIC_MODE_STATUS;

// sensors
extern String TOPIC_SENSORS;
extern String REGISTER_NEW_SENSOR;
extern String TOPIC_SENSORS_BASE;

void initTopics();

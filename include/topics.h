#pragma once
#include <Arduino.h>

// Need for MQTT connection
extern String TOPIC_STATUS;
extern String TOPIC_COMMANDS;

// extern String TOPIC_EVENTS;
// extern String TOPIC_PAIRING;

// sensors
extern String TOPIC_SENSORS;

void initTopics();

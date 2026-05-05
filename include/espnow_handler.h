#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp_now.h>

#include "message_structure.h"
#include "sensor_manager.h"
#include "mqtt_manager.h"
#include "system_state.h"
#include "topics.h"
#include "device_controller.h"

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataPtr, int len);
#include <Arduino.h>
#include "device_controller.h"
#include "mqtt_manager.h"
#include "config.h"

void armSystem()
{
    digitalWrite(LED_BUILTIN, HIGH);

    mqttManager.publish(
        TOPIC_STATUS,
        "{\"type\":\"status\",\"status\":\"armed\"}");
}

void disarmSystem()
{
    digitalWrite(LED_BUILTIN, LOW);

    mqttManager.publish(
        TOPIC_STATUS,
        "{\"type\":\"status\",\"status\":\"disarmed\"}");
}
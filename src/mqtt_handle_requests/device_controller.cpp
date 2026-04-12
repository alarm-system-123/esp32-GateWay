#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_manager.h"
#include "device_controller.h"
#include "mqtt_manager.h"
#include "topics.h"
#include "system_state.h"
#include "globals.h"

int activeGroupSensors[MAX_SENSORS];
int activeGroupSensorsCount = 0;
String deviceId = WiFi.macAddress();

void armSystem(const JsonDocument &doc)
{
    digitalWrite(LED_BUILTIN, HIGH);
    currentSystemState = ARMED_FULL;
    mqttManager.publishStatus("armed");
    preferences.putInt("mode", currentSystemState);
}

void disarmSystem(const JsonDocument &doc)
{
    digitalWrite(LED_BUILTIN, LOW);
    currentSystemState = DISARMED;
    activeGroupSensorsCount = 0;
    mqttManager.publishStatus("disarmed");
    preferences.putInt("mode", currentSystemState);
}

void statusSystem(const JsonDocument &doc)
{
    const char *system_status;

    switch (currentSystemState)
    {
    case ARMED_FULL:
        system_status = "armed";
        break;
    case ARMED_PARTIAL:
        system_status = "armed_partial";
        break;
    case ARMED_GROUP:
        system_status = "armed_group";
        break;
    case PAIRING_MODE:
        system_status = "pairing mode";
        break;
    default:
        system_status = "disarmed";
        break;
    }

    mqttManager.publishStatus(system_status);
}

void armPartial(const JsonDocument &doc)
{
    digitalWrite(LED_BUILTIN, HIGH);
    currentSystemState = ARMED_PARTIAL;
    mqttManager.publishStatus("partial armed");
    preferences.putInt("mode", currentSystemState);
}

void addSensor(const JsonDocument &doc)
{
    currentSystemState = PAIRING_MODE;
    Serial.println("System is in PAIRING_MODE");
    preferences.putInt("mode", currentSystemState);
}

void publishSingleSensor(SensorNode *node)
{
    if (node == nullptr)
        return;

    JsonDocument responseDoc;
    responseDoc["id"] = node->id;
    responseDoc["name"] = node->name;
    responseDoc["type"] = node->type;
    responseDoc["state"] = (bool)node->state;
    responseDoc["bat"] = node->batteryVolts;
    responseDoc["online"] = !sensorManager.isSensorOffline(node->id);
    responseDoc["mac"] = sensorManager.macToString(node->mac);

    String payload;
    serializeJson(responseDoc, payload);

    String topic = deviceId + "/sensors/" + String(node->id) + "/status";
    mqttManager.publish(topic.c_str(), payload.c_str());

    Serial.printf("Published to %s: %s\n", topic.c_str(), payload.c_str());
}

void sensorStatus()
{
    Serial.println("Publishing status for all sensors...");
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        SensorNode *node = sensorManager.getSensor(i);
        if (node != nullptr && node->isPaired)
        {
            publishSingleSensor(node);
        }
    }
}

void removeSensor(const JsonDocument &doc)
{
    if (!doc["id"].is<int>())
    {
        Serial.println("Error: No 'id' provided for remove_sensor");
        mqttManager.publish("home/events", "{\"event\":\"error\", \"message\":\"Missing sensor ID\"}");
        return;
    }

    int sensorId = doc["id"].as<int>();

    if (sensorId > 0)
    {
        bool success = sensorManager.removeSensorById(sensorId);

        if (success)
        {
            JsonDocument responseDoc;
            responseDoc["event"] = "sensor_removed";
            responseDoc["id"] = sensorId;

            String payload;
            serializeJson(responseDoc, payload);
            mqttManager.publish("home/events", payload.c_str());
        }
        else
        {
            mqttManager.publish("home/events", "{\"event\":\"error\", \"message\":\"Sensor not found\"}");
        }
    }
}

void updateSensorConfig(const JsonDocument &doc)
{
    if (!doc["id"].is<int>() || !doc["name"].is<const char *>())
    {
        Serial.println("Error: Missing 'id' or 'name' in update_config command");
        return;
    }

    int sensorId = doc["id"].as<int>();
    const char *newName = doc["name"].as<const char *>();

    if (sensorId > 0 && newName != nullptr)
    {
        bool success = sensorManager.updateSensorName(sensorId, newName);

        if (success)
        {
            Serial.printf("Sensor ID %d successfully renamed to '%s'\n", sensorId, newName);

            SensorNode *node = sensorManager.getSensorById(sensorId);

            if (node != nullptr)
            {
                JsonDocument responseDoc;
                responseDoc["id"] = node->id;
                responseDoc["name"] = node->name;
                responseDoc["type"] = node->type;
                responseDoc["state"] = node->state;
                responseDoc["bat"] = node->batteryVolts;

                responseDoc["online"] = !sensorManager.isSensorOffline(node->id);

                responseDoc["mac"] = sensorManager.macToString(node->mac);

                String payload;
                serializeJson(responseDoc, payload);
                String topic = "sensors/" + String(node->id) + "/status";
                mqttManager.publish(topic.c_str(), payload.c_str());
            }
        }
        else
        {
            Serial.printf("Error: Could not find paired sensor with ID %d\n", sensorId);
            mqttManager.publish("home/events", "{\"event\":\"error\", \"message\":\"Sensor not found for update\"}");
        }
    }
}
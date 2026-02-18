#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_manager.h"
#include "device_controller.h"
#include "mqtt_manager.h"
#include "topics.h"
#include "system_state.h"

void armSystem(const JsonDocument &doc)
{
    digitalWrite(LED_BUILTIN, HIGH);
    currentSystemState = ARMED_FULL;
    mqttManager.publishStatus("armed");
}

void disarmSystem(const JsonDocument &doc)
{
    digitalWrite(LED_BUILTIN, LOW);
    currentSystemState = DISARMED;
    mqttManager.publishStatus("disarmed");
}

void armPartial(const JsonDocument &doc)
{
    digitalWrite(LED_BUILTIN, HIGH);
    currentSystemState = ARMED_PARTIAL;
    mqttManager.publishStatus("partial armed");
}

void addSensor(const JsonDocument &doc)
{
    currentSystemState = PAIRING_MODE;
    Serial.println("System is in PAIRING_MODE");
}

void sensorStatus(const JsonDocument &requestDoc)
{
    Serial.println("Publishing status for all sensors...");

    for (int i = 0; i < MAX_SENSORS; i++)
    {
        SensorNode *node = sensorManager.getSensor(i);

        if (node != nullptr && node->isPaired)
        {
            JsonDocument responseDoc;
            responseDoc["id"] = node->id;
            responseDoc["name"] = node->name;
            responseDoc["type"] = node->type;
            responseDoc["state"] = node->state;
            responseDoc["bat"] = node->batteryVolts;
            responseDoc["online"] = !sensorManager.isSensorOffline(i);
            responseDoc["mac"] = sensorManager.macToString(node->mac);

            String payload;
            serializeJson(responseDoc, payload);
            String topic = "sensors/" + String(node->id) + "/status";
            mqttManager.publish(topic.c_str(), payload.c_str());

            Serial.printf("Published to %s\n", topic.c_str());
        }
    }
}

void removeSensor(const JsonDocument &doc)
{
    if (!doc.containsKey("id"))
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
            String payload = "{\"event\":\"sensor_removed\", \"id\":" + String(sensorId) + "}";
            mqttManager.publish("home/events", payload.c_str());
        }
        else
        {
            mqttManager.publish("home/events", "{\"event\":\"error\", \"message\":\"Sensor not found\"}");
        }
    }
}
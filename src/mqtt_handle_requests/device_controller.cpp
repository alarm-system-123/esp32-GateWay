#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_manager.h"
#include "device_controller.h"
#include "mqtt_manager.h"
#include "topics.h"
#include "system_state.h"

int activeGroupSensors[MAX_SENSORS];
int activeGroupSensorsCount = 0;
void armGroup(const JsonDocument &doc)
{
    // 1. Перевіряємо, чи прислали нам масив
    if (!doc["active_sensors"].is<JsonArrayConst>())
    {
        Serial.println("Error: Missing or invalid 'active_sensors' array");
        return;
    }

    JsonArrayConst sensorsArray = doc["active_sensors"].as<JsonArrayConst>();

    // 2. Проходимося по кожному ID, який прилетів у масиві
    for (JsonVariantConst v : sensorsArray)
    {
        if (!v.is<int>())
            continue; // Пропускаємо, якщо це не число

        int incomingId = v.as<int>();
        bool alreadyExists = false;

        // 3. Перевіряємо, чи немає цього датчика ВЖЕ в нашому списку охорони
        for (int i = 0; i < activeGroupSensorsCount; i++)
        {
            if (activeGroupSensors[i] == incomingId)
            {
                alreadyExists = true;
                break; // Знайшли дублікат, зупиняємо пошук для цього ID
            }
        }

        // 4. Якщо датчика ще немає в списку і є вільне місце — додаємо
        if (!alreadyExists && activeGroupSensorsCount < MAX_SENSORS)
        {
            activeGroupSensors[activeGroupSensorsCount] = incomingId;
            activeGroupSensorsCount++;
        }
    }

    // 5. Вмикаємо режим охорони для групи
    currentSystemState = ARMED_GROUP;
    digitalWrite(LED_BUILTIN, HIGH); // Вмикаємо світлодіод охорони

    Serial.printf("System ARMED (Group). Total active sensors monitoring: %d\n", activeGroupSensorsCount);

    // Сповіщаємо сервер
    mqttManager.publish("home/alarm/status", "{\"status\":\"armed_group\"}");
}

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
    activeGroupSensorsCount = 0;
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
            responseDoc["online"] = !sensorManager.isSensorOffline(node->id);
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
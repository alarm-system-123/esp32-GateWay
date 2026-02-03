#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_manager.h"
#include "device_controller.h"
#include "mqtt_manager.h"
#include "topics.h"

void armSystem()
{
    digitalWrite(LED_BUILTIN, HIGH);
    mqttManager.publishStatus("armed");
}

void disarmSystem()
{
    digitalWrite(LED_BUILTIN, LOW);
    mqttManager.publishStatus("disarmed");
}

void sensorStatus()
{
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        SensorNode *node = sensorManager.getSensor(i);

        if (node != nullptr && node->isPaired)
        {
            // Створюємо документ (розмір підбирається експериментально, 256 байт зазвичай вистачає для одного сенсора)
            JsonDocument doc;

            doc["id"] = node->id;
            doc["name"] = node->name;
            doc["type"] = node->type;
            doc["state"] = node->state;
            doc["bat"] = node->batteryVolts;
            doc["online"] = !sensorManager.isSensorOffline(i);
            doc["mac"] = sensorManager.macToString(node->mac);

            // Серіалізуємо в рядок
            String payload;
            serializeJson(doc, payload);

            String topic = "sensors/" + String(node->id) + "/status";
            mqttManager.publish(topic.c_str(), payload.c_str());
        }
    }
}
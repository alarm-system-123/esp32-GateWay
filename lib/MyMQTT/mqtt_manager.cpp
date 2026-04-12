#include "mqtt_manager.h"
#include "../../include/topics.h"
#include "../../include/config.h"
#include <ArduinoJson.h>
#include "../../include/system_state.h"
#include "../../include/device_controller.h"

void MQTTManager::begin()
{
    mqttClient.setClient(wifiClient);
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

    Serial.println("MQTT Manager initialized");
    Serial.print("Broker: ");
    Serial.print(MQTT_BROKER);
    Serial.print(":");
    Serial.println(MQTT_PORT);
}

void MQTTManager::connect()
{
    if (mqttClient.connected())
    {
        Serial.println("Already connected to MQTT broker");
        return;
    }

    Serial.print("Connecting to MQTT broker...");

    bool connected = false;

    const char *lwtMessage = "{\"gateway_status\":\"offline\",\"device\":\"main_controller\"}";

    if (strlen(MQTT_USER) > 0)
    {
        connected = mqttClient.connect(
            MQTT_CLIENT_ID,
            MQTT_USER,
            MQTT_PASSWORD,
            TOPIC_GATEWAY_STATUS.c_str(),
            1,
            true,
            lwtMessage);
    }
    else
    {
        connected = mqttClient.connect(
            MQTT_CLIENT_ID,
            TOPIC_GATEWAY_STATUS.c_str(),
            1,
            true,
            lwtMessage);
    }

    if (connected)
    {
        Serial.println(" connected!");
        mqttClient.publish(
            TOPIC_GATEWAY_STATUS.c_str(),
            "{\"gateway_status\":\"online\",\"device\":\"main_controller\"}",
            true); // true = Retained повідомлення, це дуже добре!

        subscribe(TOPIC_COMMANDS.c_str());

        switch (currentSystemState)
        {
        case ARMED_FULL:
            publishStatus("armed");
            break;
        case ARMED_PARTIAL:
            publishStatus("armed_partial");
            break;
        case ARMED_GROUP:
            publishStatus("armed_group");
            break;
        default:
            publishStatus("disarmed");
            break;
        }
        sensorStatus();
    }
    else
    {
        Serial.print(" failed! State: ");
        Serial.println(mqttClient.state());
    }
}

void MQTTManager::handle()
{
    if (mqttClient.connected())
    {
        mqttClient.loop();
    }
    else
    {
        static unsigned long lastReconnectAttempt = 0;
        unsigned long now = millis();

        if (now - lastReconnectAttempt > 5000)
        {
            lastReconnectAttempt = now;
            Serial.println("MQTT disconnected. Attempting to reconnect...");
            connect();
        }
    }
}

void MQTTManager::publish(const char *topic, const char *payload)
{
    if (!mqttClient.connected())
    {
        Serial.println("Cannot publish - MQTT not connected");
        return;
    }

    if (mqttClient.publish(topic, payload))
    {
        Serial.print("Published to ");
        Serial.print(topic);
        Serial.print(": ");
        Serial.println(payload);
    }
    else
    {
        Serial.println("Failed to publish message");
    }
}

void MQTTManager::subscribe(const char *topic)
{
    if (!mqttClient.connected())
    {
        Serial.println("Cannot subscribe - MQTT not connected");
        return;
    }

    if (mqttClient.subscribe(topic))
    {
        Serial.print("Subscribed to: ");
        Serial.println(topic);
    }
    else
    {
        Serial.print("Failed to subscribe to: ");
        Serial.println(topic);
    }
}

void MQTTManager::setCallback(MQTT_CALLBACK_SIGNATURE)
{
    mqttClient.setCallback(callback);
}

void MQTTManager::publishStatus(const char *status)
{
    JsonDocument doc;
    doc["type"] = "status";
    doc["status"] = status;

    char buffer[128];
    serializeJson(doc, buffer);

    mqttClient.publish(TOPIC_MODE_STATUS.c_str(), buffer, true);
}

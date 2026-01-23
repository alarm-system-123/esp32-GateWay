#include "mqtt_manager.h"
#include "../../include/config.h"

void MQTTManager::begin()
{
    // Ініціалізація MQTT клієнта
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

    // Підключення з або без credentials
    if (strlen(MQTT_USER) > 0)
    {
        connected = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD);
    }
    else
    {
        connected = mqttClient.connect(MQTT_CLIENT_ID);
    }

    if (connected)
    {
        Serial.println(" connected!");

        // Публікуємо статус "online"
        publish(TOPIC_STATUS, "{\"status\":\"online\",\"device\":\"main_controller\"}");

        // Підписуємось на топік команд
        subscribe(TOPIC_COMMANDS);
    }
    else
    {
        Serial.print(" failed! State: ");
        Serial.println(mqttClient.state());
        // Коди помилок:
        // -4 : MQTT_CONNECTION_TIMEOUT
        // -3 : MQTT_CONNECTION_LOST
        // -2 : MQTT_CONNECT_FAILED
        // -1 : MQTT_DISCONNECTED
        //  0 : MQTT_CONNECTED
        //  1 : MQTT_CONNECT_BAD_PROTOCOL
        //  2 : MQTT_CONNECT_BAD_CLIENT_ID
        //  3 : MQTT_CONNECT_UNAVAILABLE
        //  4 : MQTT_CONNECT_BAD_CREDENTIALS
        //  5 : MQTT_CONNECT_UNAUTHORIZED
    }
}

// bool MQTTManager::isConnected()
// {
//     return mqttClient.connected();
// }

void MQTTManager::handle()
{
    // Обробка вхідних повідомлень
    if (mqttClient.connected())
    {
        mqttClient.loop();
    }
    else
    {
        // Автоматичне перепідключення
        static unsigned long lastReconnectAttempt = 0;
        unsigned long now = millis();

        if (now - lastReconnectAttempt > 5000)
        { // Спроба кожні 5 секунд
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
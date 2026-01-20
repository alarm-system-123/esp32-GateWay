// mqtt_manager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFi.h>

class MQTTManager
{
private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;

public:
    void begin();
    void connect();
    bool isConnected();
    void handle(); // mqttClient.loop()

    void publish(const char *topic, const char *payload);
    void subscribe(const char *topic);

    void setCallback(MQTT_CALLBACK_SIGNATURE);
};

#endif
// mqtt_manager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Ethernet.h> // Замість WiFiClient.h
#include <PubSubClient.h>

class MQTTManager
{
private:
    // WiFiClient wifiClient;
    // PubSubClient mqttClient;
    EthernetClient ethClient;
    PubSubClient mqttClient;

public:
    void begin();
    void connect();
    void handle();

    void publish(const char *topic, const char *payload);
    void subscribe(const char *topic);

    void setCallback(MQTT_CALLBACK_SIGNATURE);

    void publishStatus(const char *status);
};

extern MQTTManager mqttManager;

#endif
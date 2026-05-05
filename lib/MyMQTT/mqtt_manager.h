// mqtt_manager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "../../include/topics.h"
#include "../../include/system_state.h"
#include "../../include/device_controller.h"

#define MQTT_BROKER "192.168.2.1"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "alarm_main_controller"
#define MQTT_USER "admin"
#define MQTT_PASSWORD "LwTxc12!Am"

class MQTTManager
{
private:
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
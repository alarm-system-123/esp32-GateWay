#include "espnow_handler.h"
#include <esp_now.h>
#include "message_structure.h"
#include "sensor_manager.h"
#include "mqtt_manager.h"
#include "system_state.h"

#define PAIRING_OFFSET 100

struct_message incomingData;

static void sendPairingReply(const uint8_t *macAddr)
{
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, macAddr, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (!esp_now_is_peer_exist(macAddr))
    {
        esp_now_add_peer(&peerInfo);
    }
    esp_now_send(macAddr, (uint8_t *)&incomingData, sizeof(incomingData));
}

static void sendSensorToMqtt(SensorNode *s, bool isAlarm)
{
    if (s == nullptr)
        return;

    String topic = String("home/sensors/") + String(s->id);

    String statusStr = isAlarm ? "ALARM" : "OK";
    String typeStr = (s->type == SENSOR_TYPE_IR) ? "IR" : "REED";

    String payload = "{";
    payload += "\"status\":\"" + statusStr + "\",";
    payload += "\"type\":\"" + typeStr + "\",";
    payload += "\"bat\":" + String(s->batteryVolts, 2);
    payload += "}";

    mqttManager.publish(topic.c_str(), payload.c_str());
}

bool shouldTriggerAlarm(SensorNode *s, int state)
{
    if (state == 0)
        return false;

    if (currentSystemState == ARMED_FULL)
        return true;

    if (currentSystemState == ARMED_PARTIAL)
    {
        if (s->type == SENSOR_TYPE_REED)
            return true;
    }

    return false;
}

void handlePairingRequest(const uint8_t *mac, uint8_t rawType, float battery)
{
    if (currentSystemState != PAIRING_MODE)
    {
        return;
    }

    uint8_t realType = rawType - PAIRING_OFFSET;
    int newIdx = sensorManager.registerNewSensor(mac, realType, battery);

    if (newIdx != -1)
    {
        sendPairingReply(mac);
        mqttManager.publish("home/events", "{\"event\":\"new_device_paired\"}");
    }
}

void handleKnownSensorData(int idx, const struct_message &data)
{
    sensorManager.updateSensorHeartbeat(idx, data.battery);
    sensorManager.updateSensorState(idx, data.state);

    SensorNode *s = sensorManager.getSensor(idx);
    if (s == nullptr)
        return;

    bool isAlarm = shouldTriggerAlarm(s, data.state);

    if (isAlarm)
    {
        sendSensorToMqtt(s, isAlarm);
    }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataPtr, int len)
{
    Serial.printf("\n[ESP-NOW] Прилетіло %d байт від %02X:%02X:%02X:%02X:%02X:%02X\n",
                  len, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (len != sizeof(incomingData))
    {
        Serial.printf("[ERROR] Розмір не співпадає! Очікуємо %d, а прийшло %d\n", sizeof(incomingData), len);
        return;
    }

    memcpy(&incomingData, incomingDataPtr, sizeof(incomingData));

    if (incomingData.type > PAIRING_OFFSET)
    {
        handlePairingRequest(mac, incomingData.type, incomingData.battery);
        return;
    }

    int idx = sensorManager.findSensorByMac(mac);

    if (idx != -1)
    {
        handleKnownSensorData(idx, incomingData);
    }
    else
    {
        Serial.println("Data from UNKNOWN sensor! Sending UNPAIR command.");

        struct_message unpairMsg;
        unpairMsg.type = 98;
        unpairMsg.state = 0;
        unpairMsg.battery = 0;

        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;

        if (!esp_now_is_peer_exist(mac))
        {
            esp_now_add_peer(&peerInfo);
        }
        esp_now_send(mac, (uint8_t *)&unpairMsg, sizeof(unpairMsg));
    }
}
#include "espnow_handler.h"

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

bool shouldTriggerAlarm(SensorNode *s, int state)
{
    if (state == 0)
        return false;
    if (currentSystemState == DISARMED)
        return false;
    if (currentSystemState == ARMED_FULL)
        return true;

    if (currentSystemState == ARMED_PARTIAL)
    {
        if (s->type == SENSOR_TYPE_REED)
            return true;
    }

    if (currentSystemState == ARMED_GROUP)
    {
        for (int i = 0; i < activeGroupSensorsCount; i++)
        {
            if (activeGroupSensors[i] == s->id)
            {
                Serial.printf("ALARM! Sensor ID %d is in the active group list!\n", s->id);
                return true;
            }
        }
        Serial.printf("Sensor ID %d ignored (not in armed groups).\n", s->id);
        return false;
    }
    return false;
}

void handlePairingRequest(const uint8_t *mac, uint8_t rawType, float battery)
{
    if (currentSystemState != PAIRING_MODE)
        return;

    sendPairingReply(mac);

    uint8_t realType = rawType - PAIRING_OFFSET;
    int newIdx = sensorManager.registerNewSensor(mac, realType, battery);

    if (newIdx != -1)
    {
        mqttManager.publish(REGISTER_NEW_SENSOR.c_str(), "{\"event\":\"new_device_paired\"}");
        SensorNode *newNode = sensorManager.getSensor(newIdx);
        publishSingleSensor(newNode);
    }
}
void handleKnownSensorData(int sensorId, const struct_message &data)
{
    sensorManager.updateSensorHeartbeat(sensorId, data.battery);
    sensorManager.updateSensorState(sensorId, data.state);

    SensorNode *s = sensorManager.getSensorById(sensorId);

    if (s == nullptr)
        return;

    publishSingleSensor(s);

    bool isAlarm = shouldTriggerAlarm(s, data.state);
    if (isAlarm)
    {
        Serial.printf("🚨 ТРИВОГА! Спрацював датчик: %s\n", s->name);

        JsonDocument alarmDoc;
        alarmDoc["event"] = "alarm";
        alarmDoc["sensor_id"] = s->id;
        alarmDoc["sensor_name"] = s->name;

        String alarmPayload;
        serializeJson(alarmDoc, alarmPayload);
        String alarmTopic = REGISTER_NEW_SENSOR;
        mqttManager.publish(alarmTopic.c_str(), alarmPayload.c_str());
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

    int sensorId = sensorManager.findSensorByMac(mac);

    if (sensorId != -1)
    {
        handleKnownSensorData(sensorId, incomingData);
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
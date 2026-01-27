#include "sensor_manager.h"

SensorManager::SensorManager()
{
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        sensors[i].isPaired = false;
    }
}

int SensorManager::findSensorByMac(const uint8_t *macAddr)
{
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        if (sensors[i].isPaired && memcmp(sensors[i].mac, macAddr, 6) == 0)
        {
            return i;
        }
    }
    return -1;
}

int SensorManager::registerOrUpdateSensor(const uint8_t *macAddr, uint8_t type, float battery)
{
    int index = findSensorByMac(macAddr);

    if (index != -1)
    {
        sensors[index].lastSeen = millis();
        sensors[index].batteryVolts = battery;
        return index;
    }

    for (int i = 0; i < MAX_SENSORS; i++)
    {
        if (!sensors[i].isPaired)
        {
            memcpy(sensors[i].mac, macAddr, 6);
            sensors[i].id = i + 1;
            sensors[i].type = type;
            sensors[i].isPaired = true;
            sensors[i].lastSeen = millis();
            sensors[i].batteryVolts = battery;
            sensors[i].state = false;
            snprintf(sensors[i].name, 20, "Sensor_%d", sensors[i].id);

            sensorCount++;
            return i;
        }
    }

    return -1;
}

void SensorManager::updateSensorState(int index, bool newState)
{
    if (index >= 0 && index < MAX_SENSORS)
    {
        sensors[index].state = newState;
        sensors[index].lastSeen = millis();
    }
}

SensorNode *SensorManager::getSensor(int index)
{
    if (index >= 0 && index < MAX_SENSORS && sensors[index].isPaired)
    {
        return &sensors[index];
    }
    return nullptr;
}

bool SensorManager::isSensorOffline(int index)
{
    if (!sensors[index].isPaired)
        return true;
    return (millis() - sensors[index].lastSeen) > PAIRING_TIMEOUT;
}

String SensorManager::macToString(const uint8_t *mac)
{
    char buf[18];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
}
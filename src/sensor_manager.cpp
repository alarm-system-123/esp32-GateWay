#include "sensor_manager.h"

SensorManager::SensorManager()
{
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        sensors[i].isPaired = false;
    }
}
bool SensorManager::removeSensorById(int id)
{
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        if (sensors[i].isPaired && sensors[i].id == id)
        {
            sensors[i].isPaired = false;
            sensorCount--;

            saveSensors();
            Serial.printf("Sensor ID %d successfully removed.\n", id);
            return true;
        }
    }
    Serial.printf("Error: Sensor ID %d not found.\n", id);
    return false;
}

void SensorManager::init()
{
    pref.begin("sys-data", false);
    size_t len = pref.getBytesLength("sensors_db");

    if (len == sizeof(sensors))
    {
        pref.getBytes("sensors_db", &sensors, sizeof(sensors));

        sensorCount = 0;
        for (int i = 0; i < MAX_SENSORS; i++)
        {
            if (sensors[i].isPaired)
            {
                sensorCount++;
                sensors[i].state = 0;
                sensors[i].lastSeen = millis();
            }
        }
        Serial.printf("Loaded %d paired sensors from memory.\n", sensorCount);
    }
    else
    {
        Serial.println("No saved sensors found. Starting fresh.");
    }

    pref.end();
}

void SensorManager::saveSensors()
{
    pref.begin("sys-data", false);
    pref.putBytes("sensors_db", &sensors, sizeof(sensors));
    pref.end();
    Serial.println("Sensor database saved to memory.");
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

int SensorManager::registerNewSensor(const uint8_t *macAddr, uint8_t type, float battery)
{
    int existingIdx = findSensorByMac(macAddr);
    if (existingIdx != -1)
    {
        updateSensorHeartbeat(existingIdx, battery);
        return existingIdx;
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
            sensors[i].state = 0;
            snprintf(sensors[i].name, 20, "Sensor_%d", sensors[i].id);

            sensorCount++;

            saveSensors();
            Serial.printf("New Sensor Registered: ID %d\n", sensors[i].id);

            return i;
        }
    }
    Serial.println("Error: No free slots for new sensor!");
    return -1;
}

void SensorManager::updateSensorHeartbeat(int index, float battery)
{
    if (index >= 0 && index < MAX_SENSORS && sensors[index].isPaired)
    {
        sensors[index].lastSeen = millis();
        sensors[index].batteryVolts = battery;
    }
}

void SensorManager::updateSensorState(int index, bool newState)
{
    if (index >= 0 && index < MAX_SENSORS && sensors[index].isPaired)
    {
        sensors[index].state = newState ? 1 : 0;
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
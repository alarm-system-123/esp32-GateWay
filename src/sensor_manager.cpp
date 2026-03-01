#include "sensor_manager.h"

SensorManager::SensorManager()
{
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        sensors[i].isPaired = false;
    }
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
            return sensors[i].id;
        }
    }
    return -1;
}

int SensorManager::registerNewSensor(const uint8_t *macAddr, uint8_t type, float battery)
{
    int existingId = findSensorByMac(macAddr);
    if (existingId != -1)
    {
        updateSensorHeartbeat(existingId, battery);
        return existingId;
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

            return sensors[i].id;
        }
    }
    Serial.println("Error: No free slots for new sensor!");
    return -1;
}

void SensorManager::updateSensorHeartbeat(int id, float battery)
{
    SensorNode *node = getSensorById(id);
    if (node != nullptr)
    {
        node->lastSeen = millis();
        node->batteryVolts = battery;
    }
}

void SensorManager::updateSensorState(int id, bool newState)
{
    SensorNode *node = getSensorById(id);
    if (node != nullptr)
    {
        node->state = newState ? 1 : 0;
        node->lastSeen = millis();
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

SensorNode *SensorManager::getSensorById(int id)
{
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        if (sensors[i].isPaired && sensors[i].id == id)
        {
            return &sensors[i];
        }
    }
    return nullptr;
}

bool SensorManager::isSensorOffline(int id)
{
    SensorNode *node = getSensorById(id);
    if (node == nullptr)
        return true;
    return (millis() - node->lastSeen) > PAIRING_TIMEOUT;
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

bool SensorManager::updateSensorName(int id, const char *newName)
{
    SensorNode *node = getSensorById(id);
    if (node != nullptr)
    {
        strncpy(node->name, newName, sizeof(node->name) - 1);
        node->name[sizeof(node->name) - 1] = '\0';
        saveSensors();
        return true;
    }
    return false;
}

String SensorManager::macToString(const uint8_t *mac)
{
    char buf[18];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
}
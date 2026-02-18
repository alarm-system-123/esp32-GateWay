#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h> // ДОДАНО
#include "structure_sensor.h"

#define MAX_SENSORS 10
#define PAIRING_TIMEOUT 60000

class SensorManager
{
private:
    SensorNode sensors[MAX_SENSORS];
    int sensorCount = 0;
    Preferences pref; // ДОДАНО об'єкт для роботи з пам'яттю

public:
    SensorManager();

    void init();
    void saveSensors();
    bool removeSensorById(int id);

    int findSensorByMac(const uint8_t *macAddr);
    int registerNewSensor(const uint8_t *macAddr, uint8_t type, float battery);
    void updateSensorHeartbeat(int index, float battery);
    void updateSensorState(int index, bool newState);
    SensorNode *getSensor(int index);
    bool isSensorOffline(int index);
    String macToString(const uint8_t *mac);
};

extern SensorManager sensorManager;

#endif
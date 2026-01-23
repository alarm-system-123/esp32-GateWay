// sensor_manager.h
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "config.h"

struct Sensor
{
    uint8_t mac[6];
    char id[13];
    char name[32];
    uint8_t type;
    uint8_t group_id;
    bool is_armed;
    bool is_online;
    uint8_t battery;
    uint32_t last_seen;
    bool triggered;
};

class SensorManager
{
private:
    Sensor sensors[MAX_SENSORS];
    uint8_t count;

public:
    void begin();

    int addSensor(uint8_t *mac, uint8_t type);
    Sensor *findByMAC(uint8_t *mac);
    void updateState(uint8_t *mac, bool triggered, uint8_t battery);
    void removeSensor(uint8_t *mac);

    String toJSON(); // Серіалізація для MQTT
    uint8_t getCount() { return count; }
    Sensor *getSensor(uint8_t index);
};

#endif
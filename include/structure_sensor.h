#pragma once
#include <Arduino.h>

struct SensorNode
{
    uint8_t mac[6];
    uint8_t id;
    uint8_t type;
    bool isPaired;
    bool state;
    float batteryVolts;
    unsigned long lastSeen;
    bool isReportedOffline;
    char name[20];
};
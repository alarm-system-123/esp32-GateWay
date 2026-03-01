#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <Arduino.h>
#include "sensor_manager.h"
enum SystemState
{
    DISARMED = 0,
    ARMED_FULL = 1,
    ARMED_PARTIAL = 2,
    ARMED_GROUP = 3,
    PAIRING_MODE = 99
};

extern SystemState currentSystemState;
extern int activeGroupSensors[MAX_SENSORS];
extern int activeGroupSensorsCount;

#endif
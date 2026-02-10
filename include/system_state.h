#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <Arduino.h>

enum SystemState
{
    DISARMED = 0,
    ARMED_FULL = 1,
    ARMED_PARTIAL = 2
};

extern SystemState currentSystemState;

#endif
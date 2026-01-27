#ifndef MESSAGE_STRUCTURE_H
#define MESSAGE_STRUCTURE_H

#include <Arduino.h>

#define SENSOR_TYPE_IR 0
#define SENSOR_TYPE_REED 1

typedef struct struct_message
{
    uint8_t type;
    bool state;
    float battery;
} struct_message;

#endif
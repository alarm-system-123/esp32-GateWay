#ifndef MESSAGE_STRUCTURE_H
#define MESSAGE_STRUCTURE_H

#include <Arduino.h>

#define SENSOR_TYPE_IR 1
#define SENSOR_TYPE_REED 2
#define PAIRING_OFFSET 100
typedef struct struct_message
{
    uint8_t type;
    bool state;
    float battery;
} struct_message;

#endif
#include "topics.h"
#include <WiFi.h>

static String deviceId;
String TOPIC_STATUS;
String TOPIC_SENSORS;
// String TOPIC_EVENTS;
String TOPIC_COMMANDS;
// String TOPIC_PAIRING;
void initTopics()
{
    deviceId = WiFi.macAddress();

    TOPIC_STATUS = deviceId + "/alarm/status";
    TOPIC_SENSORS = deviceId + "/alarm/sensors";
    // TOPIC_EVENTS = deviceId + "/alarm/events";
    TOPIC_COMMANDS = deviceId + "/alarm/commands";
    // TOPIC_PAIRING = deviceId + "/alarm/pairing";
}

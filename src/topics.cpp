#include "topics.h"
#include <WiFi.h>

static String deviceId;
String TOPIC_COMMANDS;
String TOPIC_GATEWAY_STATUS;
String TOPIC_MODE_STATUS;
String REGISTER_NEW_SENSOR;
String TOPIC_SENSORS_BASE;
void initTopics()
{
    deviceId = WiFi.macAddress();

    TOPIC_COMMANDS = deviceId + "/alarm/commands";
    TOPIC_GATEWAY_STATUS = deviceId + "/system/gateway/status";
    TOPIC_MODE_STATUS = deviceId + "/system/mode/status";
    REGISTER_NEW_SENSOR = deviceId + "/system/events";
    TOPIC_SENSORS_BASE = deviceId + "/sensors/";
}

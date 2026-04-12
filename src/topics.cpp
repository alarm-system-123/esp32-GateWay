#include "topics.h"
#include <WiFi.h>

static String deviceId;
String TOPIC_COMMANDS;
String TOPIC_GATEWAY_STATUS;
String TOPIC_MODE_STATUS;
void initTopics()
{
    deviceId = WiFi.macAddress();

    TOPIC_COMMANDS = deviceId + "/alarm/commands";
    TOPIC_GATEWAY_STATUS = deviceId + "/system/gateway/status";
    TOPIC_MODE_STATUS = deviceId + "/system/mode/status";
}

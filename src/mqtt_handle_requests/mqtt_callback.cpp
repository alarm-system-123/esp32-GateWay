#include "mqtt_callback.h"
#include "topics.h"
#include "json_validation.h"
#include "commands.h"
#include <ArduinoJson.h>

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    if (strcmp(topic, TOPIC_COMMANDS.c_str()) != 0)
        return;

    JsonDocument doc;

    if (!parseJson(payload, length, doc))
        return;

    handleCommand(doc);
}
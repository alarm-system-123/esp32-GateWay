#include "mqtt_callback.h"
#include "config.h"
#include "json_validation.h"
#include "commands.h"

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    if (strcmp(topic, TOPIC_COMMANDS) != 0)
        return;

    StaticJsonDocument<128> doc;
    if (!parseJson(payload, length, doc))
        return;

    handleCommand(doc);
}

#include "commands.h"
#include "device_controller.h"
#include "commandentry.h"

CommandEntry commandTable[] = {
    {"cmd", "arm", armSystem},
    {"cmd", "disarm", disarmSystem},
    {"cmd", "partial", armPartial},
    {"sensor", "sensor_status", sensorStatus}};

const size_t commandCount = sizeof(commandTable) / sizeof(commandTable[0]);

void handleCommand(const JsonDocument &doc)
{
    const char *cmd = doc["cmd"];
    const char *action = doc["action"];

    if (!cmd || !action)
    {
        Serial.println("Invalid JSON");
        return;
    }

    for (size_t i = 0; i < commandCount; i++)
    {
        if (strcmp(cmd, commandTable[i].cmd) == 0 &&
            strcmp(action, commandTable[i].action) == 0)
        {
            commandTable[i].handler();
            return;
        }
    }

    Serial.println("Unknown command");
}

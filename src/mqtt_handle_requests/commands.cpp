#include "commands.h"
#include "device_controller.h"

void handleCommand(const JsonDocument &doc)
{
    const char *cmd = doc["cmd"];
    if (!cmd)
        return;

    if (strcmp(cmd, "arm") == 0)
        armSystem();
    else if (strcmp(cmd, "disarm") == 0)
        disarmSystem();
    else
        Serial.println("Unknown command");
}

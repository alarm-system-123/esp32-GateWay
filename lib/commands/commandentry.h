#pragma once
#include <ArduinoJson.h>

typedef void (*CommandHandler)(const JsonDocument &doc);

struct CommandEntry
{
    const char *cmd;
    const char *action;
    CommandHandler handler;
};

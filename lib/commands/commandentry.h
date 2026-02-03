#pragma once
struct CommandEntry
{
    const char *cmd;
    const char *action;
    void (*handler)();
};

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <SPI.h>
#include <Ethernet.h>
class WiFiManager
{
private:
    unsigned long previousMillis = 0;
    unsigned long interval = 30000;

public:
    void begin();
    void connect();
    bool isConnected();
    void handle();
};

#endif
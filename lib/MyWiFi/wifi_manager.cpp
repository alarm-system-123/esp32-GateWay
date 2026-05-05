#include "wifi_manager.h"

#define W5500_CS_PIN 5

byte eth_mac[6];

void WiFiManager::begin()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.println("WiFi radio initialized for ESP-NOW");

    WiFi.macAddress(eth_mac);
    Serial.printf("Використовуємо MAC ESP32 для Ethernet: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  eth_mac[0], eth_mac[1], eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);

    Ethernet.init(W5500_CS_PIN);
    Serial.println("Ethernet Manager initialized");
}

void WiFiManager::connect()
{
    if (Ethernet.linkStatus() == LinkON && Ethernet.localIP() != IPAddress(0, 0, 0, 0))
    {
        Serial.println("Already connected to Ethernet");
        return;
    }

    Serial.println("Очікування апаратного старту W5500...");
    delay(1500);
    Ethernet.init(5);

    Serial.println("Connecting to Ethernet via DHCP...");

    // Передаємо нашу рідну MAC-адресу у W5500
    if (Ethernet.begin(eth_mac) == 0)
    {
        Serial.println("Failed to configure Ethernet using DHCP");

        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
            Serial.println("W5500 module not found. Check SPI wiring (CS на D5)!");
        }
        else if (Ethernet.linkStatus() == LinkOFF)
        {
            Serial.println("Ethernet cable is not connected.");
        }
    }
    else
    {
        Serial.println("\nEthernet connected!");
        Serial.print("IP address: ");
        Serial.println(Ethernet.localIP());
    }
}

bool WiFiManager::isConnected()
{
    return (Ethernet.linkStatus() == LinkON) && (Ethernet.localIP() != IPAddress(0, 0, 0, 0));
}

void WiFiManager::handle()
{
    unsigned long currentMillis = millis();

    Ethernet.maintain();

    if (!isConnected() && (currentMillis - previousMillis >= interval))
    {
        Serial.print(currentMillis);
        Serial.println(" - Ethernet disconnected. Reconnecting...");

        connect();
        previousMillis = currentMillis;
    }
}
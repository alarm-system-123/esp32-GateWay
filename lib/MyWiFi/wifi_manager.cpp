#include "wifi_manager.h"
#include "../../include/config.h"
#include <SPI.h>
#include <Ethernet.h>
#include <WiFi.h>

#define W5500_CS_PIN 5 // Пін CS для твоєї плати (D5)

byte eth_mac[6]; // Масив для зберігання рідної MAC-адреси

void WiFiManager::begin()
{
    // 1. Вмикаємо Wi-Fi радіо для ESP-NOW, але відключаємось від роутерів
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.println("WiFi radio initialized for ESP-NOW");

    // 2. Отримуємо рідну MAC-адресу ESP32 і зберігаємо в масив
    WiFi.macAddress(eth_mac);
    Serial.printf("Використовуємо MAC ESP32 для Ethernet: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  eth_mac[0], eth_mac[1], eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);

    // 3. Ініціалізуємо модуль W5500
    Ethernet.init(W5500_CS_PIN);
    Serial.println("Ethernet Manager initialized");
}

void WiFiManager::connect()
{
    // Перевіряємо, чи є лінк на кабелі і чи видана IP-адреса
    if (Ethernet.linkStatus() == LinkON && Ethernet.localIP() != IPAddress(0, 0, 0, 0))
    {
        Serial.println("Already connected to Ethernet");
        return;
    }

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
    // Мережа вважається підключеною, якщо є фізичний лінк і IP-адреса не нульова
    return (Ethernet.linkStatus() == LinkON) && (Ethernet.localIP() != IPAddress(0, 0, 0, 0));
}

void WiFiManager::handle()
{
    unsigned long currentMillis = millis();

    // Вбудована функція Ethernet для оновлення оренди IP-адреси по DHCP
    Ethernet.maintain();

    // Якщо втратили лінк (витягнули кабель або впав роутер)
    if (!isConnected() && (currentMillis - previousMillis >= interval))
    {
        Serial.print(currentMillis);
        Serial.println(" - Ethernet disconnected. Reconnecting...");

        connect(); // Намагаємося підняти мережу знову

        previousMillis = currentMillis;
    }
}
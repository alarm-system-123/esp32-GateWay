#include "wifi_manager.h"
#include "config.h"

void WiFiManager::begin()
{
    WiFi.mode(WIFI_STA);
    Serial.println("WiFi Manager initialized");
}

void WiFiManager::connect()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Already connected to WiFi");
        return;
    }

    Serial.print("Connecting to WiFi ");
    Serial.print(WIFI_SSID);
    Serial.print(" ..");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        Serial.print('.');
        delay(500);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi");
    }
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::handle()
{
    unsigned long currentMillis = millis();

    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
    {
        Serial.print(millis());
        Serial.println(" - WiFi disconnected. Reconnecting...");

        WiFi.disconnect();
        WiFi.reconnect();

        previousMillis = currentMillis;
    }
}
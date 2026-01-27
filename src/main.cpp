// common
#include <Arduino.h>
#include "config.h"

// WIFI Connection
#include "wifi_manager.h"

// ESP-NOW
#include <esp_now.h>
#include "sensor_manager.h"
#include "espnow_handler.h"

// MQTT
#include "mqtt_manager.h"
#include "./mqtt_handle_requests/mqtt_callback.h"

WiFiManager wifiManager;
MQTTManager mqttManager;
SensorManager sensorManager;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== Alarm System Main Controller ===");

  // 1. WiFi
  wifiManager.begin();
  wifiManager.connect();

  Serial.print("WiFi MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.channel());

  // 2. ESP-NOW Ініціалізація
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    delay(1000);
    ESP.restart();
  }
  else
  {
    Serial.println("ESP-NOW Initialized Successfully");
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  }

  // 3. MQTT
  mqttManager.begin();
  mqttManager.setCallback(mqttCallback);
  mqttManager.connect();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  wifiManager.handle();
  mqttManager.handle();
  delay(10);
}
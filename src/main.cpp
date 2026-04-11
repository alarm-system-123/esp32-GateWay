// common
#include "globals.h"
#include <Arduino.h>
#include "config.h"
#include "topics.h"

#include "system_state.h"

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
Preferences preferences;
SystemState currentSystemState;

void updateHardwareState()
{
  switch (currentSystemState)
  {
  case ARMED_FULL:
  case ARMED_PARTIAL:
  case ARMED_GROUP:
    digitalWrite(LED_BUILTIN, HIGH);
    break;
  case DISARMED:
    digitalWrite(LED_BUILTIN, LOW);
    break;
  }
}
void setup()
{
  Serial.begin(115200);
  delay(1000);

  // 1. НАЛАШТУВАННЯ ПІНІВ (Завжди робіть це на початку)
  pinMode(LED_BUILTIN, OUTPUT);
  // Можна короткочасно вимкнути його при старті, поки йде підключення
  digitalWrite(LED_BUILTIN, LOW);

  sensorManager.init();
  Serial.println("\n=== Alarm System Main Controller ===");

  // 2. WiFi
  wifiManager.begin();
  wifiManager.connect();

  Serial.print("WiFi MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.channel());

  // 3. ESP-NOW Ініціалізація
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

  // 4. ВІДНОВЛЕННЯ СТАНУ ТА ІНДИКАЦІЇ
  preferences.begin("security", false);
  currentSystemState = static_cast<SystemState>(preferences.getInt("mode", 0));

  // Тепер ця функція спрацює правильно, бо pinMode вже налаштовано вище!
  updateHardwareState();

  // 5. MQTT
  initTopics(); // Формуємо топіки ДО підключення
  mqttManager.begin();
  mqttManager.setCallback(mqttCallback);
  mqttManager.connect(); // Ця функція відправить відновлений стан на сервер
}

void loop()
{
  wifiManager.handle();
  mqttManager.handle();
  delay(10);
}
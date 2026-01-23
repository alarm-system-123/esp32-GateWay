// #include <Arduino.h>
// #include "config.h"
// #include "wifi_manager.h" // complete
// #include "mqtt_manager.h" // complete
// #include "espnow_manager.h"
// #include "sensor_manager.h"
// #include "pairing_manager.h"
// #include "command_handler.h"

// WiFiManager wifiManager;
// MQTTManager mqttManager;
// // ESPNowManager espnowManager;
// // SensorManager sensorManager;
// // PairingManager pairingManager;
// // CommandHandler commandHandler;

// void setup()
// {
//   Serial.begin(115200);

//   // Ініціалізація модулів
//   wifiManager.begin();
//   mqttManager.begin();
//   // espnowManager.begin();
//   // sensorManager.begin();
//   // pairingManager.begin();

//   // Підключення до WiFi
//   wifiManager.connect();

//   // Підключення до MQTT
//   mqttManager.connect();
//   mqttManager.subscribe(TOPIC_COMMANDS);
// }

// void loop()
// {
//   wifiManager.handle();
//   mqttManager.handle();
//   // pairingManager.handle();

//   // Періодична відправка статусу
//   // static uint32_t lastStatusSend = 0;
//   // if (millis() - lastStatusSend > 30000)
//   // { // Кожні 30 сек
//   //   String sensorsJSON = sensorManager.toJSON();
//   //   mqttManager.publish(TOPIC_SENSORS, sensorsJSON.c_str());
//   //   lastStatusSend = millis();
//   // }
// }

// common
#include <Arduino.h>
#include "config.h"
// WIFI Connection
#include "wifi_manager.h"

// MQTT
#include "mqtt_manager.h"
#include "./mqtt_handle_requests/mqtt_callback.h"

WiFiManager wifiManager;
MQTTManager mqttManager;

void temp_func()
{
  // Тестова публікація кожні 10 секунд
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 30000)
  {
    String payload = "{\"uptime\":" + String(millis() / 1000) + "}";
    mqttManager.publish(TOPIC_STATUS, payload.c_str());
    lastPublish = millis();
  }
}
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== Alarm System Main Controller ===");

  // 1. WiFi
  wifiManager.begin();
  wifiManager.connect();

  // 2. MQTT
  mqttManager.begin();
  mqttManager.setCallback(mqttCallback); // Встановлюємо callback
  mqttManager.connect();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  wifiManager.handle();
  mqttManager.handle();

  temp_func();

  delay(10);
}
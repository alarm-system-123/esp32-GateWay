#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h" // complete
#include "mqtt_manager.h"
#include "espnow_manager.h"
#include "sensor_manager.h"
#include "pairing_manager.h"
#include "command_handler.h"

WiFiManager wifiManager;
MQTTManager mqttManager;
// ESPNowManager espnowManager;
// SensorManager sensorManager;
// PairingManager pairingManager;
// CommandHandler commandHandler;

void setup()
{
  Serial.begin(115200);

  // Ініціалізація модулів
  wifiManager.begin();
  mqttManager.begin();
  // espnowManager.begin();
  // sensorManager.begin();
  // pairingManager.begin();

  // Підключення до WiFi
  wifiManager.connect();

  // Підключення до MQTT
  mqttManager.connect();
  mqttManager.subscribe(TOPIC_COMMANDS);
}

void loop()
{
  wifiManager.handle();
  mqttManager.handle();
  // pairingManager.handle();

  // Періодична відправка статусу
  // static uint32_t lastStatusSend = 0;
  // if (millis() - lastStatusSend > 30000)
  // { // Кожні 30 сек
  //   String sensorsJSON = sensorManager.toJSON();
  //   mqttManager.publish(TOPIC_SENSORS, sensorsJSON.c_str());
  //   lastStatusSend = millis();
  // }
}
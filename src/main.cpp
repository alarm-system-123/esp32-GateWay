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

#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

WiFiManager wifiManager;
MQTTManager mqttManager;

// Callback для обробки вхідних MQTT повідомлень
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  // Перетворення payload в string
  String message = "";
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Обробка команд
  if (strcmp(topic, TOPIC_COMMANDS) == 0)
  {
    // Тут буде обробка JSON команд
    if (message == "arm")
    {
      Serial.println("Command: ARM system");
    }
    else if (message == "disarm")
    {
      Serial.println("Command: DISARM system");
    }
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
}

void loop()
{
  wifiManager.handle();
  mqttManager.handle();

  // Тестова публікація кожні 10 секунд
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 10000)
  {
    String payload = "{\"uptime\":" + String(millis() / 1000) + "}";
    mqttManager.publish(TOPIC_STATUS, payload.c_str());
    lastPublish = millis();
  }

  delay(10);
}
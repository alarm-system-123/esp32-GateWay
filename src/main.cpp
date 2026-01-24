// common
#include <Arduino.h>
#include <esp_now.h> // 1. Додаємо бібліотеку ESP-NOW
#include "config.h"
#include "sensor_manager.h" // 2. Додаємо наш менеджер датчиків

// WIFI Connection
#include "wifi_manager.h"

// MQTT
#include "mqtt_manager.h"
#include "./mqtt_handle_requests/mqtt_callback.h"

WiFiManager wifiManager;
MQTTManager mqttManager;
SensorManager sensorManager; // 3. Створюємо об'єкт менеджера

// --- Структура повідомлення (Має співпадати з кодом на датчику ESP32-C3) ---
typedef struct struct_message
{
  uint8_t type;  // Тип: 0 - IR, 1 - Reed
  bool state;    // true - Тривога, false - Спокій
  float battery; // Напруга батареї
} struct_message;

struct_message incomingData;

// --- Функція обробки вхідних даних по ESP-NOW ---
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataPtr, int len)
{
  // Перевіряємо, чи коректний розмір даних
  if (len != sizeof(incomingData))
  {
    Serial.println("Error: Wrong data size received");
    return;
  }

  memcpy(&incomingData, incomingDataPtr, sizeof(incomingData));

  // 1. Реєструємо датчик у менеджері (він сам створить його або знайде існуючий)
  int idx = sensorManager.registerOrUpdateSensor(mac, incomingData.type, incomingData.battery);

  if (idx != -1)
  {
    // 2. Оновлюємо стан датчика
    sensorManager.updateSensorState(idx, incomingData.state);

    // 3. Отримуємо актуальні дані про датчик для відправки в MQTT
    SensorNode *s = sensorManager.getSensor(idx);

    if (s != nullptr)
    {
      // Формуємо топік: наприклад "home/sensors/1/status"
      // (Припускаємо, що TOPIC_STATUS визначений у config.h, наприклад "home/alarm")
      String topic = String("home/sensors/") + String(s->id);

      // Формуємо JSON для зручності
      String payload = "{";
      payload += "\"status\":\"" + String(s->state ? "ALARM" : "OK") + "\",";
      payload += "\"type\":\"" + String(s->type == SENSOR_TYPE_IR ? "IR" : "REED") + "\",";
      payload += "\"bat\":" + String(s->batteryVolts, 2);
      payload += "}";

      // Відправляємо в MQTT
      mqttManager.publish(topic.c_str(), payload.c_str());

      Serial.print("ESP-NOW Recv from ID:");
      Serial.print(s->id);
      Serial.print(" -> MQTT Payload: ");
      Serial.println(payload);
    }
  }
}

void temp_func()
{
  // Heartbeat шлюзу (щоб знати, що сама база жива)
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 30000)
  {
    String payload = "{\"uptime\":" + String(millis() / 1000) + ", \"wifi_rssi\":" + String(WiFi.RSSI()) + "}";
    // TOPIC_STATUS має бути в config.h
    mqttManager.publish("home/gateway/status", payload.c_str());
    lastPublish = millis();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== Alarm System Main Controller ===");

  // 1. WiFi (Важливо: ESP-NOW потребує активного WiFi інтерфейсу)
  wifiManager.begin();
  wifiManager.connect();

  Serial.print("WiFi MAC: ");
  Serial.println(WiFi.macAddress());

  // 2. ESP-NOW Ініціалізація
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    // Якщо ESP-NOW не стартував, перезавантажуємо, бо це критично
    delay(1000);
    ESP.restart();
  }
  else
  {
    Serial.println("ESP-NOW Initialized Successfully");
    // Реєструємо функцію зворотного виклику
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

  temp_func();

  delay(10);
}
// common
#include "globals.h"
#include <Arduino.h>
#include "config.h"
#include "topics.h"
#include "device_controller.h"
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

unsigned long lastSensorReportTime = 0;
const unsigned long SENSOR_REPORT_INTERVAL = 3600000;

// Налаштування: якщо датчик мовчить більше 70 хвилин (годину спить + 10 хв запас)
const unsigned long OFFLINE_THRESHOLD = 4200000;
unsigned long lastWatchdogCheck = 0;
const unsigned long WATCHDOG_INTERVAL = 30000;

void checkSensorsHealth()
{
  if (millis() - lastWatchdogCheck < WATCHDOG_INTERVAL)
    return;
  lastWatchdogCheck = millis();

  for (int i = 0; i < MAX_SENSORS; i++)
  {
    SensorNode *s = sensorManager.getSensor(i);
    if (s != nullptr && s->isPaired)
    {

      bool isTimeout = (millis() - s->lastSeen > OFFLINE_THRESHOLD);

      if (isTimeout && !s->isReportedOffline)
      {
        s->isReportedOffline = true;
        Serial.printf("⚠️ Sensor %d is OFFLINE\n", s->id);
        sensorStatus();
      }
      else if (!isTimeout && s->isReportedOffline)
      {
        s->isReportedOffline = false;
        Serial.printf("✅ Sensor %d is back ONLINE\n", s->id);
      }
    }
  }
}

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

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  sensorManager.init();
  Serial.println("\n=== Alarm System Main Controller ===");

  // 2. WiFi
  wifiManager.begin();
  wifiManager.connect();

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

  updateHardwareState();

  // 5. MQTT
  initTopics();
  mqttManager.begin();
  mqttManager.setCallback(mqttCallback);
  mqttManager.connect();
}

void loop()
{
  wifiManager.handle();
  mqttManager.handle();

  checkSensorsHealth();

  if (millis() - lastSensorReportTime >= SENSOR_REPORT_INTERVAL)
  {
    lastSensorReportTime = millis();
    sensorStatus();
  }
}
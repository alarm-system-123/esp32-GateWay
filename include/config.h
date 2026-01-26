#ifndef CONFIG_H
#define CONFIG_H

// WiFi
#define WIFI_SSID "Luda"         // Luda_5G ipxone
#define WIFI_PASSWORD "05031960" // 05031960 rolton1575
#define WIFI_CHANNEL 1

// MQTT
#define MQTT_BROKER "192.168.0.101" // 192.168.0.101
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "alarm_main_controller"
#define MQTT_USER ""
#define MQTT_PASSWORD ""

// Topics
#define TOPIC_STATUS "alarm/status"
#define TOPIC_SENSORS "alarm/sensors"
#define TOPIC_EVENTS "alarm/events"
#define TOPIC_COMMANDS "alarm/commands"
#define TOPIC_PAIRING "alarm/pairing"

// ESP-NOW it's esp staff
#define MAX_SENSORS 10
#define PAIRING_TIMEOUT 60000 // 60 секунд

// Sensor types
#define SENSOR_TYPE_IR 0
#define SENSOR_TYPE_REED 1

#endif
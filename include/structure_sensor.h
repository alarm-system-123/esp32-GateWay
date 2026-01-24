#pragma once
#include <Arduino.h>

struct SensorNode
{
    uint8_t mac[6];         // Унікальна адреса (паспорт датчика)
    uint8_t id;             // Внутрішній ID (0...MAX_SENSORS) для MQTT топіків
    uint8_t type;           // SENSOR_TYPE_IR або SENSOR_TYPE_REED
    bool isPaired;          // true, якщо слот зайнятий і датчик зареєстрований
    bool state;             // Поточний стан: true (Тривога/Відкрито), false (Спокій/Закрито)
    float batteryVolts;     // Напруга батареї (наприклад, 4.12 V)
    unsigned long lastSeen; // Час останнього контакту (millis()) для перевірки онлайну
    char name[20];          // Зручна назва, наприклад "Kitchen"
};
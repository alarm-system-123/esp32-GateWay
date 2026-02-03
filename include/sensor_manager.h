#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <WiFi.h> // Потрібно для роботи з macAddress
#include "structure_sensor.h"

#define MAX_SENSORS 10
#define PAIRING_TIMEOUT 60000 // 60 секунд

// --- Клас для управління всіма датчиками ---
class SensorManager
{
private:
    SensorNode sensors[MAX_SENSORS]; // Масив всіх можливих датчиків
    int sensorCount = 0;

public:
    SensorManager();

    // Знайти датчик за MAC-адресою. Повертає індекс в масиві або -1, якщо не знайдено.
    int findSensorByMac(const uint8_t *macAddr);
    // Додати новий датчик або оновити існуючий
    int registerOrUpdateSensor(const uint8_t *macAddr, uint8_t type, float battery);

    // Оновити статус тривоги (Motion або Door Open)
    void updateSensorState(int index, bool newState);

    // Отримати вказівник на датчик за індексом (для читання даних)
    SensorNode *getSensor(int index);

    // Перевірка на "мертві" датчики (Offline)
    bool isSensorOffline(int index);

    // Допоміжна функція для форматування MAC в рядок (для логів)
    String macToString(const uint8_t *mac);
};

extern SensorManager sensorManager;

#endif
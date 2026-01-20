// espnow_manager.h
#ifndef ESPNOW_MANAGER_H
#define ESPNOW_MANAGER_H

#include <esp_now.h>

// Структура повідомлень
struct ESPNowMessage
{
    uint8_t type; // 0=pairing, 1=event, 2=heartbeat, 3=test_mode
    uint8_t sensor_type;
    bool triggered;
    uint8_t battery;
    uint32_t timestamp;
};

class ESPNowManager
{
public:
    void begin();
    void onDataReceived(const uint8_t *mac, const uint8_t *data, int len);
    void sendCommand(uint8_t *mac, uint8_t command);
};

#endif
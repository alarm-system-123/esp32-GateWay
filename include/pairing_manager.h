// pairing_manager.h
#ifndef PAIRING_MANAGER_H
#define PAIRING_MANAGER_H

#include <Arduino.h>

enum PairingMode
{
    PAIRING_DISABLED,
    PAIRING_AUTO,
    PAIRING_SPECIFIC
};

class PairingManager
{
private:
    PairingMode mode;
    uint8_t expected_mac[6];
    uint32_t timeout;

public:
    void begin();
    void startAuto(uint32_t duration);
    void startSpecific(uint8_t *mac, uint32_t duration);
    void stop();
    bool canPair(uint8_t *mac);
    bool isActive();
    void handle(); // Перевірка timeout в loop()
};

#endif
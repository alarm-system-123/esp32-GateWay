#include "espnow_handler.h"
#include "message_structure.h"

#include "sensor_manager.h"
#include "mqtt_manager.h"

struct_message incomingData;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataPtr, int len)
{
    if (len != sizeof(incomingData))
    {
        Serial.println("Error: Wrong data size received");
        return;
    }

    memcpy(&incomingData, incomingDataPtr, sizeof(incomingData));

    int idx = sensorManager.registerOrUpdateSensor(mac, incomingData.type, incomingData.battery);

    if (idx != -1)
    {
        sensorManager.updateSensorState(idx, incomingData.state);

        SensorNode *s = sensorManager.getSensor(idx);

        if (s != nullptr)
        {
            String topic = String("home/sensors/") + String(s->id);

            String payload = "{";
            payload += "\"status\":\"" + String(s->state ? "ALARM" : "OK") + "\",";
            payload += "\"type\":\"" + String(s->type == SENSOR_TYPE_IR ? "IR" : "REED") + "\",";
            payload += "\"bat\":" + String(s->batteryVolts, 2);
            payload += "}";

            mqttManager.publish(topic.c_str(), payload.c_str());

            Serial.print("ESP-NOW Recv from ID:");
            Serial.print(s->id);
            Serial.print(" -> MQTT Payload: ");
            Serial.println(payload);
        }
    }
}
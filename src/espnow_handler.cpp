#include "espnow_handler.h"
#include "message_structure.h"

#include "sensor_manager.h"
#include "mqtt_manager.h"
#include "system_state.h"

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
            if (currentSystemState == ARMED_FULL)
            {
                sendSensorToMqtt(s);
            }
            else if (currentSystemState == ARMED_PARTIAL)
            {
                if (s->type == SENSOR_TYPE_REED)
                    sendSensorToMqtt(s);
            }
        }
    }
}

static void sendSensorToMqtt(SensorNode *s)
{
    if (s == nullptr)
        return;

    String topic = String("home/sensors/") + String(s->id);

    String statusStr = "ALARM";
    String typeStr = (s->type == SENSOR_TYPE_IR) ? "IR" : "REED";

    String payload = "{";
    payload += "\"status\":\"" + statusStr + "\",";
    payload += "\"type\":\"" + typeStr + "\",";
    payload += "\"bat\":" + String(s->batteryVolts, 2);
    payload += "}";

    mqttManager.publish(topic.c_str(), payload.c_str());

    Serial.print("ID:");
    Serial.print(s->id);
    Serial.print(" [State: ");
    Serial.print(statusStr);
    Serial.print("] -> Payload: ");
    Serial.println(payload);
}
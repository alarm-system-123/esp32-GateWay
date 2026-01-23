#include "json_validation.h"

bool parseJson(byte *payload,
               unsigned int length,
               StaticJsonDocument<128> &doc)
{
    payload[length] = '\0';

    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return false;
    }

    return true;
}

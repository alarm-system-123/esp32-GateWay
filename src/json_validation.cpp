#include "json_validation.h"

bool parseJson(byte *payload,
               unsigned int length,
               JsonDocument &doc)
{
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error)
    {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return false;
    }

    return true;
}

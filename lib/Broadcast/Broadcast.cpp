#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif



#include "Broadcast.h"
#include "secret.txt"

const char *ssid = STASSID;
const char *password = STAPSK;

Broadcast::Broadcast()
{
}

void Broadcast::Setup()
{
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    int retry = 0;
    bool connected = true;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        retry++;
        if (retry > 5)
        {
            connected = false;
            break;
        }
    }
    if (connected)
    {
        Serial.println("");
        IPAddress ip = WiFi.localIP();
        Serial.println("WiFi connected. Local IP " + ip.toString());
        
    }
    else
    {
        Serial.println("Unabled to connect to the WiFi. Disconnected.");
    }
}

void Broadcast::SendData(String data)
{
}
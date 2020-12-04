#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include "Uploader.h"
#include "secret.txt"

const char *ssid = STASSID;
const char *password = STAPSK;
const char *host = HOSTFORCONN;
const int port = PORTFORCONN;
const char fingerprint[] PROGMEM = FINGERPRINTMYSRV;
const char *guidsensor = SENSOR_GUID_MY;
const char *pubLink = PUBDATALINK;

Uploader::Uploader()
{
}

void Uploader::Setup()
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

void Uploader::SendData(String data)
{
    WiFiClientSecure httpsClient;
    Serial.printf("Using fingerprint '%s'\n", fingerprint);

    httpsClient.setFingerprint(fingerprint);

    httpsClient.setTimeout(7000);
    int r = 0; //retry counter
    int max_retry = 3;
    while ((!httpsClient.connect(host, port)) && (r < max_retry))
    {
        delay(100);
        Serial.print(".");
        r++;
    }
    if (r == max_retry)
    {
        Serial.println("Connection failed");
        return;
    }
    else
    {
        Serial.println("Connected to web");
    }
    Serial.print("requesting URL: ");
    Serial.println(host);

    String getData, Link;

    //POST Data
    httpsClient.print(String("POST ") + pubLink + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
                     "DeviceToken: " + guidsensor  + "\r\n" +
                     "Content-Length: " + String(data.length()) + "\r\n\r\n" +
                     data + "\r\n" +
                     "Connection: close\r\n\r\n");
    Serial.println("closing connection");
}
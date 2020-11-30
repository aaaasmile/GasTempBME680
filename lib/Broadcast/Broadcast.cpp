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
const char *host = HOSTFORCONN;
const int port = PORTFORCONN;

Broadcast::Broadcast()
{
    this->_bufPos = 0;
}

void Broadcast::Setup()
{
    // Connect to WiFi network
    //delay(1000);
    //WiFi.mode(WIFI_OFF); //Prevents reconnection issue (taking too long to connect)
    //delay(1000);
    //WiFi.mode(WIFI_STA); //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    int retry = 0;
    bool connected = true;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1500);
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
    this->_connected = connected;
}

String g_buffer[10];

void Broadcast::SendData(String data)
{
    Serial.print("Send Data");
    int buf_max_size = 10;
    int buf_size_sent = 4;
    if (this->_bufPos < buf_max_size)
    {
        g_buffer[this->_bufPos] = data;
        this->_bufPos++;
        if (this->_bufPos < buf_size_sent)
        {
            return;
        }
    }
    else
    {
        g_buffer[this->_overPos] = data;
        this->_overPos++;
        if (this->_overPos >= buf_max_size)
        {
            this->_overPos = 0;
        }
    }

    WiFiClient httpClient;
    httpClient.setTimeout(3000 * (buf_size_sent - 1));
    int r = 0; //retry counter
    while ((!httpClient.connect(host, port)) && (r < 30))
    {
        delay(100);
        Serial.print(".");
        r++;
    }
    if (r == 30)
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
    Link = "/iot/PubData";

    httpClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
                     "Content-Length: 13" + "\r\n\r\n" +
                     "say=Hi&to=Mom" + "\r\n" +
                     "Connection: close\r\n\r\n");
    Serial.println("request sent");

    while (httpClient.connected())
    {
        String line = httpClient.readStringUntil('\n');
        if (line == "\r")
        {
            Serial.println("headers received");
            break;
        }
    }

    Serial.println("reply was:");
    Serial.println("==========");
    String line;
    while (httpClient.available())
    {
        line = httpClient.readStringUntil('\n'); //Read Line by Line
        Serial.println(line);                    //Print response
    }
    Serial.println("==========");
    Serial.println("closing connection");

    this->_bufPos = 0;
    this->_overPos = 0;
}
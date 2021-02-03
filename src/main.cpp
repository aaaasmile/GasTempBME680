#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>

#include <MyLight.h>
#include <BoschMgr.h>
#include <Uploader.h>

MyLight *myLight;
BoschMgr *boschMgr;
Uploader *uploader;

const int analogInPin = A0;
const bool g_debug = true;

void setup()
{
  myLight = new MyLight(g_debug);
  myLight->Setup();
  myLight->TurnOn();

  boschMgr = new BoschMgr(false);

  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println(F("My BME680 test, version 0.1.2"));

  uploader->Setup();
  boschMgr->Setup();
  myLight->CheckLeds();
  delay(500);
  myLight->TurnOff();
}

void loop()
{
  float iaq = boschMgr->Next(g_debug); // polling, should be below 3 sec. Data are povided every 3000ms

  int analogValue = analogRead(analogInPin);
  if (analogValue > 700)
  {
    if (g_debug)
    {
      Serial.println("Analog value " + String(analogValue) + " trigger display");
    }
    myLight->LightTheState();
  }

  myLight->UpdateLight(iaq);
  bool connFailed = uploader->SendData(boschMgr->GetData(), g_debug); // collect data only if the sensor was read. Upate to server after collecting 3k of data.
  if (connFailed)
  {
    Serial.println("Connection issue...");
    myLight->LightConnectionError();
  }
  else
  {
    delay(100);
  }
}
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>

#include <MyLight.h>
#include <BoschMgr.h>
#include <Broadcast.h>

MyLight *myLight;
BoschMgr *boschMgr;
Broadcast *broadCaster;

int loopCount = 0;

void setup()
{
  myLight = new MyLight();
  myLight->Setup();
  myLight->TurnOn();

  boschMgr = new BoschMgr();

  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println(F("My BME680 test, version 0.0.1"));

  broadCaster->Setup();
  boschMgr->Setup();
  myLight->CheckLeds();
  delay(500);
  myLight->TurnOff();
}

void loop()
{
    float iaq = boschMgr->Next();

    delay(3000);

    myLight->UpdateLight(iaq);
    broadCaster->SendData(boschMgr->GetData());
}
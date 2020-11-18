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

const int analogInPin = A0; 


void setup()
{
    myLight = new MyLight();
    myLight->Setup();
    myLight->TurnOn();

    boschMgr = new BoschMgr(false);

    Serial.begin(115200);
    while (!Serial)
      ;
    Serial.println(F("My BME680 test, version 0.1.2"));

    broadCaster->Setup();
    boschMgr->Setup();
    myLight->CheckLeds();
    delay(500);
    myLight->TurnOff();
}

void loop()
{
    float iaq = boschMgr->Next();

    for (size_t i = 0; i < 6; i++)
    {
        delay(500);
        int analogValue = analogRead(analogInPin);
        if (analogValue > 700){
          Serial.println("Analog value " + String(analogValue) + " trigger display");
          myLight->LightTheState();
        }
        
    }
    
    myLight->UpdateLight(iaq);
    broadCaster->SendData(boschMgr->GetData());
}
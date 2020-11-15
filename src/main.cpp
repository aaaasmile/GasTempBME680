#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <MyLight.h>
#include <BoschMgr.h>

MyLight *myLight;
BoschMgr *boschMgr;

int loopCount = 0;

void setup()
{
  myLight = new MyLight();
  myLight->Setup();
  myLight->TurnOn();

  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println(F("My BME680 test"));

  boschMgr->Setup();
}

void loop()
{
  boschMgr->Loop();

  delay(2000);

  myLight->UpdateLight(loopCount);
  loopCount++;
  if (loopCount > 10)
  {
    loopCount = 0;
    myLight->TurnOff();
  }
}
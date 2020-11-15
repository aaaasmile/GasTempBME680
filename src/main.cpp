#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <MyLight.h>
#include <MyAdaBme.h>

MyLight *myLight;
MyAdaBme *myAdaBme;

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

  myAdaBme->Setup();
}

void loop()
{
  myAdaBme->Loop();
  
  delay(2000);

  myLight->UpdateLight(loopCount);
  loopCount++;
  if (loopCount > 10)
  {
    loopCount = 0;
    myLight->TurnOff();
  }
}
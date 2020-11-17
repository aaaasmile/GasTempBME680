#include <Arduino.h>
#include "MyLight.h"

int LedRedpin = D5;
int LedGreenpin = D6;
int LedYellowpin = D7;

MyLight::MyLight()
{
    this->_greenState = false;
    this->_yellowState = false;
    this->_redState = false;
    this->_stateOn = false;
}

void MyLight::Setup()
{
    pinMode(LedRedpin, OUTPUT);
    pinMode(LedGreenpin, OUTPUT);
    pinMode(LedYellowpin, OUTPUT);
}

void MyLight::TurnOn()
{
    this->_loopSameVal = 0;
    this->_stateOn = true;
}

void MyLight::TurnOff()
{
    if (this->_stateOn)
    {
        this->turnRed(false);
        this->turnYellow(false);
        this->turnGreen(false);
    }
    this->_stateOn = false;
}

void MyLight::CheckLeds()
{
    this->turnRed(true);
    this->turnYellow(true);
    this->turnGreen(true);
}

int MyLight::calculateIAQScore(float iaq) {
  String IAQ_text = "air quality is ";
  int res = 0;

  if      (iaq > 301){               IAQ_text += "Hazardous"; res = 6; }
  else if (iaq > 250 && iaq <= 300 ){ IAQ_text += "Very Unhealthy"; res = 5;}
  else if (iaq > 200 && iaq <= 250 ){ IAQ_text += "More than Unhealthy";res = 4;}
  else if (iaq > 150 && iaq <= 200 ){ IAQ_text += "Unhealthy";res = 3;}
  else if (iaq > 100 && iaq <= 150 ){ IAQ_text += "Unhealthy for Sensitive Groups"; res = 2;}
  else if (iaq >  50 && iaq <= 100 ){ IAQ_text += "Moderate"; res = 1;}
  else if (iaq >=  00 && iaq <=  50 ){ IAQ_text += "Good";res = 0;}
  Serial.println("IAQ Score = " + String(res) + ", " + IAQ_text );

  return res;
}

void MyLight::UpdateLight(float iaq)
{
    Serial.printf("UpdateLight %f ", iaq);

    int oldScore = this->_iaqScore;
    this->_iaqScore = this->calculateIAQScore(iaq);
    if (this->_iaqScore == oldScore){
        this->_loopSameVal++;
    }else{
        this->TurnOn();
    }
    if (this->_loopSameVal > 10){
        this->TurnOff();
        return;
    }

    if (!this->_stateOn)
    {
        return;
    }
    if (this->_iaqScore <= 1)
    {
        // green
        this->turnRed(false);
        this->turnYellow(false);
        this->turnGreen(true);
    }
    else if (this->_iaqScore <= 2)
    {
        // gree/yellow
        this->turnRed(false);
        this->turnYellow(true);
        this->turnGreen(true);
    }
    else if (this->_iaqScore <= 3)
    {
        // yellow
        this->turnRed(false);
        this->turnYellow(true);
        this->turnGreen(false);
    }
    else if (this->_iaqScore <= 5)
    {
        // yellow / red
        this->turnRed(true);
        this->turnYellow(true);
        this->turnGreen(false);
    }
    else 
    {
        // red
        this->turnRed(true);
        this->turnYellow(false);
        this->turnGreen(false);
    }
}

void MyLight::turnGreen(bool state)
{
    if (this->_greenState == state)
    {
        return;
    }
    if (state)
    {
        digitalWrite(LedGreenpin, HIGH);
        Serial.println("GREEN light to High");
    }
    else
    {
        digitalWrite(LedGreenpin, LOW);
    }
    this->_greenState = state;
}

void MyLight::turnYellow(bool state)
{
    if (this->_yellowState == state)
    {
        return;
    }

    if (state)
    {
        digitalWrite(LedYellowpin, HIGH);
        Serial.println("YELLOW light to High");
    }
    else
    {
        digitalWrite(LedYellowpin, LOW);
    }
    this->_yellowState = state;
}

void MyLight::turnRed(bool state)
{
    if (this->_redState == state)
    {
        return;
    }

    if (state)
    {
        digitalWrite(LedRedpin, HIGH);
        Serial.println("RED light to High");
    }
    else
    {
        digitalWrite(LedRedpin, LOW);
    }
    this->_redState = state;
}
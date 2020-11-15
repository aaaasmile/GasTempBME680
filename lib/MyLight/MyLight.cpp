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
}

void MyLight::Setup()
{
    pinMode(LedRedpin, OUTPUT);
    pinMode(LedGreenpin, OUTPUT);
    pinMode(LedYellowpin, OUTPUT);
}

void MyLight::UpdateLight(int state)
{
    if (state <= 5)
    {
        this->turnRed(false);
        this->turnYellow(false);
        this->turnGreen(true);
    }
    else if (state <= 6)
    {
        this->turnRed(false);
        this->turnYellow(true);
        this->turnGreen(true);
    }
    else if (state <= 8)
    {
        this->turnRed(false);
        this->turnYellow(true);
        this->turnGreen(false);
    }
    else if (state <= 10)
    {
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
        Serial.print("GREEN light to High");
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
        Serial.print("YELLOW light to High");
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
        Serial.print("RED light to High");
    }
    else
    {
        digitalWrite(LedRedpin, LOW);
    }
    this->_redState = state;
}
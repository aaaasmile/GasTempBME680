#ifndef MYLIGHT_H
#define MYLIGHT_H

class MyLight
{
public:
    MyLight();
    void Setup();
    void UpdateLight(float iaq);
    void TurnOn();
    void TurnOff();
    void CheckLeds();

private:
    void turnGreen(bool state);
    void turnYellow(bool state);
    void turnRed(bool state);
    int calculateIAQScore(float iaq);

    bool _redState;
    bool _yellowState;
    bool _greenState;
    bool _stateOn;

    int _iaqScore;
    int _loopSameVal;
};

#endif
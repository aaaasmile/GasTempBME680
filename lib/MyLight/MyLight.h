#ifndef MYLIGHT_H
#define MYLIGHT_H

class MyLight
{
public:
    MyLight(bool debug);
    void Setup();
    void UpdateLight(float iaq);
    void LightConnectionError();
    void TurnOn();
    void TurnOff();
    void CheckLeds();
    void LightTheState();

private:
    void turnGreen(bool state);
    void turnYellow(bool state);
    void turnRed(bool state);
    int calculateIAQScore(float iaq);
    void lightTheState();

    bool _redState;
    bool _yellowState;
    bool _greenState;
    bool _stateOn;

    int _iaqScore;
    int _loopSameVal;
    bool _debug;
    bool _on_error;
};

#endif
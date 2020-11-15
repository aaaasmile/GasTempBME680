#ifndef MYLIGHT_H
#define MYLIGHT_H

class MyLight
{
public:
    MyLight();
    void Setup();
    void UpdateLight(int state);
    void TurnOn();
    void TurnOff();

private:
    void turnGreen(bool state);
    void turnYellow(bool state);
    void turnRed(bool state);

    bool _redState;
    bool _yellowState;
    bool _greenState;
    bool _stateOn;
};

#endif
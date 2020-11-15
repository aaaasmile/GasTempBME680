
class MyLight
{
public:
    MyLight();
    void Setup();
    void UpdateLight(int state);

private:
    void turnGreen(bool state);
    void turnYellow(bool state);
    void turnRed(bool state);

    bool _redState;
    bool _yellowState;
    bool _greenState;
};
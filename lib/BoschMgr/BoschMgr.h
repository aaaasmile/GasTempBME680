#ifndef BOSCH_MGR_H
#define BOSCH_MGR_H

#include "WString.h"

class BoschMgr
{
public:
    BoschMgr(bool useErpomState);
    void Setup();
    float Next(bool debug);
    String GetData();

private:
    void loadState();
    void updateState();
    void checkIaqSensorStatus(bool debug);
    void errLeds();

    String _valSensors;
    uint16_t _stateUpdateCounter;
    bool _useEpromState;

    float _co2Equivalent;
    float _iaq;
};

#endif

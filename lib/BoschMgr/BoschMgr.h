#ifndef BOSCH_MGR_H
#define BOSCH_MGR_H

#include "WString.h"

class BoschMgr
{
public:
    BoschMgr(bool useErpomState);
    void Setup();
    float Next();
    String GetData() { return this->_valSensors; }

private:
    void loadState();
    void updateState();
    void checkIaqSensorStatus();
    void errLeds();

    String _valSensors;
    uint16_t _stateUpdateCounter;
    bool _useEpromState;

    float _co2Equivalent;
    float _iaq;
};

#endif

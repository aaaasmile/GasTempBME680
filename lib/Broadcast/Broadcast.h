#ifndef _BROADCAST_H__
#define _BROADCAST_H_

#include "WString.h"

class Broadcast
{
public:
    Broadcast();

    void SendData(String data);
    void Setup();

private:
    bool _connected;
    int _bufPos;
    int _overPos;
};

#endif

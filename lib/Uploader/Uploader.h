#ifndef _UPLOADER_H_
#define _UPLOADER_H_

#include "WString.h"

class Uploader
{
public:
    Uploader();

    bool SendData(String data, bool debug);
    void Setup();
private:
    /* data */

};


#endif

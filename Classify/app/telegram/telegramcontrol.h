#ifndef TELEGRAMCONTROL_H
#define TELEGRAMCONTROL_H
#include "abscontrol.h"
#include "telegram.h"

class TELEGRAMControl:public absControl
{
public:
    TELEGRAMControl(TELEGRAM* t):app(t){}
    void ProcessFile(string file, string filename, string fileext);
    void ProcessSession(Json::Value jbody);
private:
    TELEGRAM* app;
};

#endif // TELEGRAMCONTROL_H

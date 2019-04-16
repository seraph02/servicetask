#ifndef TELEGRAMCONTROL_H
#define TELEGRAMCONTROL_H
#include "abscontrol.h"
#include "telegram.h"
class TELEGRAMControl:public absControl
{
public:
    TELEGRAMControl(TELEGRAM* t):telegram(t){}
private:
    TELEGRAM* telegram;
};

#endif // TELEGRAMCONTROL_H

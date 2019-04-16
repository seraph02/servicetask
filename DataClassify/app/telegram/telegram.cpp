#include "telegram.h"
#include "telegramcontrol.h"
#include <glog/logging.h>
TELEGRAM::TELEGRAM()
{
//    LOG(INFO) <<"TELEGRAM START!";
    this->control = new TELEGRAMControl(this);
}
TELEGRAM::~TELEGRAM()
{
//    LOG(INFO) <<"TELEGRAM END!";
    if(this->control!=NULL)
        delete this->control;
}

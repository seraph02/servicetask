#include "voxer.h"
#include "voxercontrol.h"
#include <glog/logging.h>
VOXER::VOXER()
{
//    LOG(INFO) <<"VOXER START!" ;
    this->control = new VOXERControl(this);
}
VOXER::~VOXER()
{
//    LOG(INFO) <<"VOXER END!";
    if(this->control!=NULL)
        delete this->control;
}

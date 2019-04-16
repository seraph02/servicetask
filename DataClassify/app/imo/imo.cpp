#include "imo.h"
#include "imocontrol.h"
#include <glog/logging.h>
IMO::IMO()
{
//    LOG(INFO) <<"IMO START!" ;
    this->control = new IMOControl(this);
}
IMO::~IMO()
{
//    LOG(INFO) <<"IMO END!";
    if(this->control!=NULL)
        delete this->control;
}

#include "skype.h"
#include "skypecontrol.h"
SKYPE::SKYPE()
{
//    cout <<"The Object Name is \"FBM\" construced!" << endl;
    this->control = new SKYPEControl(this);
}
SKYPE::~SKYPE()
{
//    cout <<"The Object Name is \"FBM\" destroyed!" << endl;
    if(this->control!=NULL)
        delete this->control;
}


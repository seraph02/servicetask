#include "fbm.h"
#include "fbmcontrol.h"
FBM::FBM()
{
//    cout <<"The Object Name is \"FBM\" construced!" << endl;
    this->control = new FBMControl(this);
}
FBM::~FBM()
{
//    cout <<"The Object Name is \"FBM\" destroyed!" << endl;
    if(this->control!=NULL)
        delete this->control;
}


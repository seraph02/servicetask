#ifndef FBMCONTROL_H
#define FBMCONTROL_H
#include "abscontrol.h"
#include "fbm.h"
#include <jsoncpp/json/json.h>

class FBMControl:public absControl
{
public:
    FBMControl(FBM*f):app(f){}
private:
    FBM* app;
    int id;
private:

    virtual void filteravatar(Json::Value&);
//    virtual void filtermessage(Json::Value&);
};

#endif // FBMCONTROL_H

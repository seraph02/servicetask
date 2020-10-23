#ifndef SKYPECONTROL_H
#define SKYPECONTROL_H
#include "abscontrol.h"
#include "skype.h"
#include <jsoncpp/json/json.h>

class SKYPEControl:public absControl
{
public:
    SKYPEControl(SKYPE*f):app(f){}
private:
    SKYPE* app;
    int id;
private:
    virtual void ProcessFile(string file, string filename, string fileext);
    virtual void ProcessMessage(Json::Value jbody);
};

#endif // FBMCONTROL_H

#ifndef VOXERCONTROL_H
#define VOXERCONTROL_H
#include "abscontrol.h"
#include "voxer.h"
#include <jsoncpp/json/json.h>
class VOXERControl:public absControl
{
public:
    VOXERControl(VOXER* voxer):app(voxer){}
private:
    virtual void filtermessage(Json::Value& jones);
    virtual void filteravatar(Json::Value& jones);

    VOXER* app;
};

#endif // VOXERCONTROL_H

#ifndef IMOCONTROL_H
#define IMOCONTROL_H
#include "abscontrol.h"
#include "imo.h"
#include <jsoncpp/json/json.h>
class IMOControl:public absControl
{
public:
    IMOControl(IMO* imo):app(imo){}
private:
    virtual void filteravatar(Json::Value& jones);
    virtual void ProcessMessage(Json::Value jbody);
    IMO* app;
};
#endif // IMOCONTROL_H

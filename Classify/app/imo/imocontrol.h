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
    void filteravatar(Json::Value& jones);
    IMO* app;
};
#endif // IMOCONTROL_H

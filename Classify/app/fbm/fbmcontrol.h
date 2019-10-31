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
    string getkey(string data,string key);
    string getkey4reg(string data,string regex);
    void ProcessGroup(Json::Value jbody);
    //void GroupTxt();
    //virtual void work();
    virtual void filteravatar(Json::Value&);
    virtual void ProcessFile(string file, string filename, string fileext);
//    virtual void filtermessage(Json::Value&);
};

#endif // FBMCONTROL_H

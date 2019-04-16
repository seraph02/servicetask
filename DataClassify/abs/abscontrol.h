#ifndef ABSCONTROL_H
#define ABSCONTROL_H
#include"absresult.h"
#include <jsoncpp/json/json.h>
class absControl
{
public:
    virtual void set(string b){body = b;}
    virtual void work();
    virtual void body2files();
    virtual absresult* produce()
    {
        return& rst;
    }

    virtual ~absControl(){}
protected:
    virtual void filteravatar(Json::Value&){}
    virtual void filtermessage(Json::Value&){}
    virtual void ProcessContacts(Json::Value jbody);
    virtual void ProcessMessage(Json::Value jbody);
protected:
    string body;
    vector<string> files;
    absresult rst;
};
int DOWNLOAD_FILE(const char* url,const char* outfilename);
vector<string> path2files(string path);
#endif // ABSCONTROL_H

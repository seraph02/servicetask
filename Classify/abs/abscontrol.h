#ifndef ABSCONTROL_H
#define ABSCONTROL_H
#include"absresult.h"
#include <jsoncpp/json/json.h>
#include <sstream>
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
    virtual void filterdialog(Json::Value&){}
    virtual void filterchannel(Json::Value&){}
    virtual void ProcessContacts(Json::Value jbody);
    virtual void ProcessDialog(Json::Value jbody);
    virtual void ProcessChannel(Json::Value jbody);
    virtual void ProcessMessage(Json::Value jbody);
    virtual void ProcessFile(string file,string filename,string fileext);
    virtual void ProcessArray(Json::Value jbody,string tname,int mode);
    string getkey(string data,string key);
    string getkey4reg(string data,string regex);
protected:
    string body;
    vector<string> files;
    absresult rst;
    string dt = gettimetag();

private:
    string gettimetag()
    {
        ostringstream oss;//创建一个流
        time_t time_now; time(&time_now);
        oss<<time_now;
        return oss.str();

    }
};
int DOWNLOAD_FILE(const char*  url,const char* outfilename);
vector<string> path2files(string path);
#endif // ABSCONTROL_H

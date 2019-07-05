#ifndef MYSELFMANAGE_H
#define MYSELFMANAGE_H

#include "comm.h"
#include <iostream>
#include "myselfinfo.h"
#include "conf.pb.h"
#include "esinfo.pb.h"
#include "devinfo.pb.h"
using namespace std;
using namespace SCPROTO;
#define BaiDuURL "http://www.baidu.com/"
#define GoogleURL "http://www.google.com/"
#define MAX_TIMEOUT 2       //about 3m
#define MAX_PROXYCOUNT 10   
#define CHANGEPROXYJARPATH  "changeproxy.jar"
struct PROXYSTATUS
{
    int timeoutcount =0;
    int id=0;
};
class Manager_Info : public IManager
{
public:

    void Update(int state);
    virtual void run();
    virtual ~Manager_Info();
    void DevProcess();
    void DevComplete();
    void DevError();
    void Updatetasklist(string appname);

    bool GetNetInfo(CStatus* infoMNG);
    bool GetProxyInfo(CStatus* infoMNG);
    bool GetESInfo(CStatus* infoMNG);
    bool GetNetDiskInfo(CStatus* infoMNG);
    bool GetDevInfo(DevInfo* infoMNG);
    void ChangeProxy(int proxyid);
    static Manager_Info* getInstance()
    {
        return infoMNG;
    }
    static void ChangeHealth(MyHealth* info)
    {
        health = info;
    }
private:
    inline bool CheckStatus(EType stype){ return ((status>>stype)&1); }

private:

    int status=0x0;

    PROXYSTATUS proxystatus;
    
    static MyHealth *health;
    static Manager_Info* infoMNG;
};



#endif // MYSELF_H

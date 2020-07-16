#ifndef MYSELFINFO_H
#define MYSELFINFO_H
#include <list>
#include "comm.h"
#include "base.h"
#include <map>
#include "obmanager.h"
#include "IInfoBase.h"
#include "devinfo.pb.h"
using namespace std;
using namespace SCPROTO;
#define def_jobs "telegramcontrol,telegram,fbmcontrol,voxercontrol,"
class MyHealth : public IHealth
{

private:
    std::list<IManager *> m_ObserverList;
    int status = 0x0;
    map<string,string> jobs_map;
    vector<string> jobs_list;
    string GetIMEI();
    string GetMAC();
    string GetLocalIP();
    string GetProxyIP();
    string GetDevName();
    string GetJobs();
public:
    DevInfo* b_dev=NULL;
public:

    MyHealth();
    ~MyHealth();
    static MyHealth* health;
    int GetFlag(){return status;}
    void SetDevInfo(DevInfo *info);
    void SetDevInfo(string strinfo);
inline bool CheckStatus(EType stype){ return ((status>>stype)&1); }


    void SetState(EType stype,EStatus state)
    {
        int tmp = state ==0?(status&(~(1<<stype))):(status|(1<<stype));
        bool ischange = (tmp == status?false:true);
        status = tmp;
        if(ischange) this->Notify();
    }
    void Attach(IManager *pObserver)
    {
        m_ObserverList.push_back(pObserver);
    }

    void Detach(IManager *pObserver)
    {
        m_ObserverList.remove(pObserver);
    }

    void Notify()
    {
        std::list<IManager *>::iterator it = m_ObserverList.begin();
        while (it != m_ObserverList.end())
        {
            (*it)->Update(status);
            ++it;
        }
    }
    static MyHealth* getInstance()
    {
        return health;
    }

};

#endif // NETINFO_H

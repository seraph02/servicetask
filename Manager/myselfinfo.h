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
#define def_jobs "export,telegramcontrol,telegram,fbmcontrol,voxercontrol,twittercontrol,"
class MyHealth : public IHealth
{

private:
    std::list<IManager *> m_ObserverList;
    map<string,string> jobs_map;
    vector<string> jobs_list;
    string GetIMEI();
    string GetMAC();
    string GetLocalIP();
    //string GetProxyIP();
    string GetDevName();
    string GetJobs();
public:
    DevInfo* b_dev = NULL;
    string GetProxyIP();
    string GetProxyIPAdr(string ip);
public:

    MyHealth();
    ~MyHealth();
    static MyHealth* health;
    int GetFlag(){return b_dev->flag();}
    void SetDevInfo(DevInfo *info);
    void SetDevInfo(string strinfo);
inline bool CheckStatus(EType stype){ return ((b_dev->flag()>>stype)&1); }
    void UPDATA();

    void SetState(EType stype,EStatus state)
    {
        int tmp = (state==0)?(b_dev->flag()&(~(1<<stype))):(b_dev->flag()|(1<<stype));
        bool ischange = (tmp == b_dev->flag()?false:true);
        b_dev->set_flag(tmp);
        if(ischange)
            this->Notify();

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
            (*it)->Update(b_dev->flag());
            ++it;
        }
    }

    static MyHealth* getInstance()
    {
        return health;
    }

};

#endif // NETINFO_H

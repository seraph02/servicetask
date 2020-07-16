#include "taskinfo.pb.h"
#include "manager_core.h"
#include "manager_es.h"
#include "manager_info.h"
#include "manager_task.h"
#include "manager_listen.h"
#include "jsoncpp2pb.h"
#include "conf.h"
#include "taskunit.h"
#include "myselfinfo.h"
#include <glog/logging.h>
#include <boost/algorithm/string.hpp>
using namespace SCPROTO;


bool Manager_Core::Init()
{
    if(MyHealth::getInstance()->b_dev->ip().empty()) return false;
    ConfInfo* conf = Manager_conf::getInstance();
    LOG(INFO)<<MyHealth::getInstance()->b_dev->ip();
    //conf->set_eshost(m_info.b_dev->ip());
    vector<string> vec;
    boost::split(vec, conf->eshost(),boost::is_any_of("\n"), boost::token_compress_on);
    for(int i = 0; i < vec.size(); ++i)
    {

        vec[i]="http://"+vec[i]+":"+conf->esport()+"/";

        //cout<<vec[i]<<endl;
    }
    Manager_ES::getInstance()->ChangeHosts(vec);

    MyHealth::getInstance()->Attach(Manager_Info::getInstance());
    m_SelfTimer->AsyncLoop(10000,SelfCHK_Heartbeat_Thread,this);//

    MyHealth::getInstance()->Attach(Manager_Task::getInstance());
    m_WorkTimer->AsyncLoop(10000,Work_Heartbeat_Thread,this);

    Manager_Listen::getInstance()->Init(13695);


    return true;
}

void Manager_Core::SelfCHK_Heartbeat_Thread(void *arg)
{
    Manager_Info::getInstance()->run();
}

void Manager_Core::Work_Heartbeat_Thread(void *arg)
{
//    LOG(INFO)<<"work heartbeat "<<endl;
//    Manager_Task::getInstance()->run();

    if(m_tasklist.size()>0) return;
    string taskfilename = Manager_conf::getInstance()->tasktmppath();
    absTask* taskunit =new TaskUnit;
    m_tasklist.push_back(taskunit);
    taskunit->setpath4task(taskfilename);
    taskunit->gettask();
    taskunit->showtask();
    taskunit->run();
    taskunit->after();
    m_tasklist.remove(taskunit);
//    taskunit.after();
}

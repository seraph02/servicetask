#include "taskinfo.pb.h"
#include "manager_core.h"
#include "manager_es.h"
#include "manager_info.h"
#include "manager_task.h"
#include "jsoncpp2pb.h"
#include "conf.h"
#include "taskunit.h"
#include <glog/logging.h>
using namespace SCPROTO;


bool Manager_Core::Init()
{
    if(m_info.b_dev->ip().empty()) return false;
    ConfInfo* conf = Manager_conf::getInstance();
    LOG(INFO)<<m_info.b_dev->ip();
    conf->set_eshost(m_info.b_dev->ip());
    Manager_ES::ChangeHosts({"http://"+conf->eshost()+":"+conf->esport()+"/"});

    Manager_Info::ChangeHealth(&m_info);
    m_info.Attach(Manager_Info::getInstance());
    m_SelfTimer->AsyncLoop(10000,SelfCHK_Heartbeat_Thread,this);//

    Manager_Task::ChangeHealth(&m_info);
    m_info.Attach(Manager_Task::getInstance());
    m_WorkTimer->AsyncLoop(10000,Work_Heartbeat_Thread,this);


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

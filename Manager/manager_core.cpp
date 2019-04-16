#include "taskinfo.pb.h"
#include "manager_core.h"
#include "manager_es.h"
#include "manager_info.h"
#include "manager_task.h"
#include "jsoncpp2pb.h"
#include "conf.h"

using namespace SCPROTO;


void Manager_Core::Init()
{
    ConfInfo* conf = Manager_conf::getInstance();
    Manager_ES::ChangeHosts({"http://"+conf->eshost()+":"+conf->esport()+"/"});

    Manager_Info::ChangeHealth(&m_info);
    m_info.Attach(Manager_Info::getInstance());
    m_SelfTimer->AsyncLoop(10000,SelfCHK_Heartbeat_Thread,this);//

    Manager_Task::ChangeHealth(&m_info);
    m_info.Attach(Manager_Task::getInstance());
    m_WorkTimer->AsyncLoop(10000,Work_Heartbeat_Thread,this);


}

void Manager_Core::SelfCHK_Heartbeat_Thread(void *arg)
{
    Manager_Info::getInstance()->run();
}

void Manager_Core::Work_Heartbeat_Thread(void *arg)
{
//    LOG(INFO)<<"work heartbeat "<<endl;
    Manager_Task::getInstance()->run();
}

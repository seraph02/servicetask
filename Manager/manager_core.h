#ifndef SERVICEMANAGE_H
#define SERVICEMANAGE_H
#include "myselfinfo.h"
#include "ctimer.hpp"
#include "conf.pb.h"
#include "devinfo.pb.h"
#include"abstask.h"
#include<list>

class Manager_Core
{

private:
    CTimer *m_SelfTimer= new CTimer("selfchecking");
    CTimer *m_WorkTimer= new CTimer("work");
    static void SelfCHK_Heartbeat_Thread(void *arg);		//selfchecking定时器任务函数
    static void Work_Heartbeat_Thread(void *arg);          //work定时器任务函数
public:
    bool Init();
    static Manager_Core* getInstance() {
        return core;
    }

private:
    static Manager_Core* core;
    static list<absTask*> m_tasklist;
};
Manager_Core* Manager_Core::core = new Manager_Core;
list<absTask*> Manager_Core::m_tasklist;
#endif // SERVICEMANAGE_H

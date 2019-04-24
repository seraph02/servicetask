#include "taskunit.h"
#include "manager_task.h"
#include "manager_info.h"
TaskUnit::TaskUnit()
{

}
void TaskUnit::gettask(){
//m_taskInffile()
//open local
   Manager_Task::GetTaskInfo(this);
}
void TaskUnit::run()
{

    try
    {
        if(t_task.id().empty()) return;

//task run
//devinfo.process++;
        Manager_Info::getInstance()->DevProcess();
        if(!Manager_Task::CheckTask(this)) return;

        while(t_task.status()<TaskInfo_TaskStatus_Complete)
        {
            Manager_Task::TaskLoops(this);
        }
        if(t_task.status()==TaskInfo_TaskStatus_Complete)
        {
//task after
            bool Isafter = Manager_Task::TaskAfter(this);
            if(Isafter)
            {
//devinfo.complete++;
                Manager_Info::getInstance()->DevComplete();
            }
            else
            {
                Manager_Info::getInstance()->DevError();
            }
        }

    }
    catch(exception& e)
    {
        LOG(ERROR)<<"workmanage:dowork "<<e.what();
    }
}
void TaskUnit::after(){
    Manager_Task::TaskAfter(this);
}


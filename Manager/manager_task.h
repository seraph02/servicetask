#ifndef WORKMANAGE_H
#define WORKMANAGE_H

#include "obmanager.h"
#include "IInfoBase.h"
#include "taskinfo.pb.h"
#include "myselfinfo.h"
#include "conf.pb.h"
#include "conf.h"

using namespace SCPROTO;
class ServiceTask
{

};
class Manager_Task:public IManager
{
public:
    void Update(int state)
    {
        status = state;
        m_IsStop = !(CheckStatus(BaseStatus)&&CheckStatus(ProxyStatus)&&CheckStatus(ESStatus)&&CheckStatus(NetDiskStatus))&&CheckStatus(AdbStatus);
        LOG(WARNING)   <<"status :"<< CheckStatus(AdbStatus) << "."
                                   << CheckStatus(BaseStatus) << "."
                                   << CheckStatus(ProxyStatus) << "."
                                   << CheckStatus(ESStatus)<< "."
                                   << CheckStatus(NetDiskStatus) << "."
                                      ;
    }
    virtual void run();
    virtual ~Manager_Task(){}
    inline bool CheckStatus(EType stype){ return ((status>>stype)&1); }

    bool CheckTimeOut(TaskInfo* info);
    bool CheckRunning(TaskInfo* info);
    bool CheckSubsequent(TaskInfo info);

    bool GetTaskInfo(TaskInfo* info);

    bool ReadLocalTask(TaskInfo* info);
    bool WriteLocalTask(TaskInfo info);

    bool PUSHRemoteResult(string info,string taskid,string indices,string resultjson);
    bool PUSHRemoteFiles(string info,string taskid,string indices,TaskResult& result);
    bool PUSHRemoteDataCF(string info,TaskInfo* task,string strkey,string indices ,string resultjson);//taskid  //update datacount

    void TaskLoops(TaskInfo* info);
    void TaskProcess(TaskInfo info);
    bool TaskAfter(TaskInfo info);

    static Manager_Task* getInstance() {    return m_taskMNG;   }
    static void ChangeHealth(MyHealth* health) { b_info = health; }

private:
    static MyHealth *b_info;
    static Manager_Task* m_taskMNG;
    int status = 0x0;
    bool m_IsStop=true;
    inline string m_workID(){                       return b_info->b_dev->nodeid().c_str();   }
    inline string m_taskInffile(){                  return (Manager_conf::getInstance()->tasktmppath()+"task.tsk").c_str();    }
    inline const string m_taskRstfile(string key){  return (Manager_conf::getInstance()->tasktmppath()+ "Result." + key).c_str();    }
    string dcfexename ="dataclassify";
    ServiceTask st;
};



#endif // WORKMANAGE_H

#ifndef WORKMANAGE_H
#define WORKMANAGE_H

#include "obmanager.h"
#include "IInfoBase.h"
#include "taskinfo.pb.h"
#include "myselfinfo.h"
#include "conf.pb.h"
#include "conf.h"

#include "abstask.h"

using namespace SCPROTO;
#define TIME_OUT 3600

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

    static bool CheckTimeOut(absTask* task);
    bool CheckRunning(TaskInfo* info);
    static bool CheckSubsequent(absTask* task);
    static bool IsChangeRemotStop(absTask* task);
    static bool GetTaskInfo(absTask* task);

    static bool ReadLocalTask(absTask* task);
    static bool WriteLocalTask(absTask* task);

    static bool PUSHRemoteResult(string info,string taskid,string indices,string resultjson);
    static bool PUSHRemoteFiles(string info,string taskid,string indices,TaskResult& result);
    static bool PUSHRemoteDataCF(string info,TaskInfo* task,string strkey,string indices ,string resultjson);//taskid  //update datacount

    static void TaskLoops(absTask* task);
    static void TaskProcess(absTask* task);
    static bool TaskAfter(absTask* task);

    static bool CheckTask(absTask* task);

    static Manager_Task* getInstance() {    return m_taskMNG;   }
    static void ChangeHealth(MyHealth* health) { b_info = health; }

private:
    static string TaskBegin(string appname,string& args);
    static MyHealth *b_info;
    static Manager_Task* m_taskMNG;
    int status = 0x0;
    static bool m_IsStop;
    static inline string m_workID(){                       return b_info->b_dev->nodeid().c_str();   }
//    inline string gettaskInffile(){                  return (Manager_conf::getInstance()->tasktmppath()+"task.tsk").c_str();    }
    static inline const string m_taskRstfile(string key){  return (Manager_conf::getInstance()->tasktmppath()+ "Result." + key).c_str();    }
    static string dcfexename;

};



#endif // WORKMANAGE_H

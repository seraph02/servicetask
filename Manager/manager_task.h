#ifndef WORKMANAGE_H
#define WORKMANAGE_H

#include "obmanager.h"
#include "IInfoBase.h"
#include "taskinfo.pb.h"
#include "myselfinfo.h"
#include "conf.pb.h"
#include "conf.h"
#include <jsoncpp/json/json.h>
#include "abstask.h"

using namespace SCPROTO;
#define TIME_OUT 3600

class Manager_Task:public IManager
{
public:
    void Update(int state)
    {
        status = state;
        m_IsStop = !(CheckStatus(Local)&&CheckStatus(Proxy)&&CheckStatus(ES)&&CheckStatus(NetDisk)&&CheckStatus(Adb))||CheckStatus(Offline);
        LOG(WARNING)   <<"status :"<< CheckStatus(Adb) << "."
                                   << CheckStatus(Local) << "."
                                   << CheckStatus(Proxy) << "."
                                   << CheckStatus(ES)<< "."
                                   << CheckStatus(NetDisk) << "."
                                   << !CheckStatus(Offline) << "."
                                      ;
    }
    virtual void run();
    virtual ~Manager_Task(){}
    bool CheckStatus(EType stype){ return ::CheckStatus(stype,status); }

    static bool CheckTimeOut(absTask* task);
    bool CheckRunning(TaskInfo* info);
    static bool CheckSubsequent(absTask* task);
    static bool IsChangeRemotStop(absTask* task);
    static bool IsChangeRemotDelete(absTask* task);
    static bool IsChangeTaskOwn(absTask* task);
    static void TaskFilter(Json::Value & json);
    static bool GetTaskInfo(absTask* task);

    static bool ReadLocalTask(absTask* task);
    static bool WriteLocalTask(absTask* task);

    static bool PUSHRemoteResult(string info,string taskid,string indices,string resultjson);
    static bool PUSHRemoteFiles(string info,string taskid,string indices,TaskResult& result);
    static bool RunDataclassify( string info,string taskid,string strkey,string indices ,string resultjson);
//    static int PUSHRemoteDataCF( string info,string taskid,string strkey,string indices ,string resultjson);
    static void TaskLoops(absTask* task);
    static void TaskProcess(absTask* task);
    static bool TaskAfter(absTask* task);
    static bool CheckTask(absTask* task);

    static Manager_Task* getInstance() {    return m_taskMNG;   }
    //static void ChangeHealth(MyHealth* health) { b_info = health; }

private:
    static string TaskBegin(string appname,string& args);
    //static MyHealth *b_info;
    static Manager_Task* m_taskMNG;
    int status = 0x0;
    static bool m_IsStop;
    static inline string m_workID(){                       return MyHealth::getInstance()->b_dev->nodeid().c_str();   }
//    inline string gettaskInffile(){                  return (Manager_conf::getInstance()->tasktmppath()+"task.tsk").c_str();    }
    static inline const string m_taskRstfile(string key){  return (Manager_conf::getInstance()->tasktmppath()+ "Result." + key).c_str();    }
    static string dcfexename;

};



#endif // WORKMANAGE_H

#include <comm.h>
#include "manager_es.h"
#include "manager_task.h"
#include "manager_info.h"
#include <jsoncpp2pb.h>
#include "bin2ascii.h"
#include <boost/algorithm/string.hpp>
using namespace std;
using google::protobuf::Message;
using namespace SCPROTO;
Manager_Task* Manager_Task::m_taskMNG = new Manager_Task;
MyHealth* Manager_Task::b_info = new MyHealth;
inline int GetTaskTol(TaskInfo info)
{
    return info.key().size()>0?info.key_size() * info.info_size():info.info_size();;
}

inline string GetKey(TaskInfo info)
{
    if(info.key_size()<1) return "";
    int keyid = info.info_size()!=0?info.progress()/info.info_size():0;
    if(keyid>info.info_size()) keyid-=1;
    string tmp = info.key(keyid);
    boost::trim_left(tmp);//去掉字符串左边空格
    boost::trim_right(tmp);//去掉字符串右边空格
    return tmp;
}
inline string GetInfo(TaskInfo info)
{
    if(info.info_size()<1) return "";
    int infoid = info.info_size()!=0?info.progress()%info.info_size():0;
    return info.info(infoid);
}
inline string GetAppName(string strinfo)
{
    vector<string> strsubarray =vector<string>();
    SplitString(strinfo.c_str(),strsubarray,"::");
    return strsubarray.size()>1?strsubarray[0].c_str():strinfo.c_str();
}
inline bool CheckTask(TaskInfo* info)
{
    bool bolret = false;
    if(info->info_size()<1)
    {
        info->set_status(TaskInfo_TaskStatus_Complete);
    }
    bolret = true;
    return bolret;
}

void Manager_Task::run()
{
    TaskInfo t_task;
    try
    {
//open local
        bool IsTask = GetTaskInfo(&t_task);
        if(!IsTask) return;
//task run
//devinfo.process++;
        Manager_Info::getInstance()->DevProcess();
        if(!CheckTask(&t_task)) return;

        while(t_task.status()<TaskInfo_TaskStatus_Complete)
        {
            TaskLoops(&t_task);
        }
        if(t_task.status()==TaskInfo_TaskStatus_Complete)
        {
//task after
            bool Isafter = TaskAfter(t_task);
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

bool Manager_Task::CheckSubsequent(TaskInfo info)
{
    if(GetTaskTol(info)>0 && info.progress() < GetTaskTol(info))
    {
        string strtask = GetInfo(info);
        string strkey = GetKey(info);
        string strApp = GetAppName(strtask);
        string strret = ReadLocalFile(m_taskRstfile(strApp+strkey).c_str());
        if(strret.size()<=0) return false;
    }
    return true;
}

void Manager_Task::TaskProcess(TaskInfo info)
{
    string strcmd="";
    ostringstream ocmd;    
    ocmd << "java -jar " +Manager_conf::getInstance()->pocpath();
    if(GetTaskTol(info)>0 && info.progress() <= GetTaskTol(info))
    {
        string strtask = GetInfo(info);
        vector<string> strsubarray =vector<string>();
        SplitString(strtask.c_str(),strsubarray,"::");
        string strApp = strtask;
        string strargs="";
        if(strsubarray.size()>1)
        {
            strApp = strsubarray[0];
            ostringstream args;
            for(int i=1;i<strsubarray.size();i++)
            {
                args << strsubarray[i] <<"::";
            }
            string tmp = args.str();
            if(tmp.size()>2)
            {
                strargs = tmp.substr(0,tmp.size()-2);
            }
        }

        string strkey =GetKey(info);
        ocmd << strApp  << " -t=" +strApp << " -k=" << strkey << " " <<strargs;
        strcmd = ocmd.str();
        string strret;
        bool Isretry=true;
        for(int in=0;in<MAXRETRY;in++)
        {
            LOG(INFO)<<"runshell :"<<strcmd.c_str();
            strret= RunShell(strcmd.c_str());
            LOG(INFO)<< " => " << strret;
            if(strret.compare("-1")!=0 &&strret.size()>1)
            {
                Isretry=false;
                Manager_Info::getInstance()->Updatetasklist(strApp);
                break;
            }
            //sleep(10);
        }

        if(Isretry){strret = "retry RunShell error"; }
        string resultjson ="";
        try
        {
            Json::Value root;Json::Reader reader;
            if (reader.parse(strret, root)&&root.isObject())  // reader将Json字符串解析到root，root将包含Json里所有子元素
            {
                resultjson = strret;
            }
            else
            {
                throw exception();
            }
        }
        catch(exception &e)
        {
            Json::Value tmp;
            tmp["status"]="Error";
            tmp["message"]=strret;
            resultjson =tmp.toStyledString();
        }
        WriteLocalFile(m_taskRstfile(strApp+strkey).c_str(),resultjson);
    }
}

bool Manager_Task::PUSHRemoteResult(string info,string taskid,string indices,string resultjson)
{
    bool ret = false;

    //task info set stat in ES
    string putjson = resultjson;
    try
    {
//post jar result
        Json::Value jsondata;
        jsondata["task"]=info;
        jsondata["taskid"]=taskid;
        jsondata["result"]=putjson.c_str();

        Json::FastWriter jfw;
        std::string strpostdata=jfw.write(jsondata);
        Manager_ES::getInstance()->POSTTaskResult(indices,strpostdata);
        ret = true;
    }
    catch(exception& e)
    {
        LOG(ERROR)<<"CURL ERROR :"<<indices<<e.what();
    }

    return ret;
}

bool Manager_Task::TaskAfter(TaskInfo info)
{
    if(remove(m_taskInffile().c_str())==0) { LOG(INFO)<<"clean taskfile !"; }
    try
    {
        TaskInfo changetask;
        changetask.set_id(info.id());
        changetask.set_status(info.status());
        changetask.set_etime(info.etime());
        //task info set stat in ES
        Manager_ES::getInstance()->UpdateTaskInfo(info.id(),pb2json(changetask));
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what();
    }
    return true;
}

void resultAddfiles(TaskResult* result,string strjson)
{
    //{"contactsJsonPath":"/home/twd/sn/imocontroller/result/13201398522//contacts.json","status":"OK","messageJsonPath":"/home/twd/sn/imocontroller/result/13201398522//contacts.json"}
        try
        {
            Json::Value resjson; Json::Reader jrd;
            if(!jrd.parse(strjson,resjson)||resjson.isNull()) return;

            Json::Value::Members mem = resjson.getMemberNames();
            for (auto iter = mem.begin(); iter != mem.end(); iter++)
            {
                std::string strkey = *iter;
                std::size_t found = strkey.rfind("Path");
                if (found==std::string::npos) continue;
                //have *Path field
                Json::Value PathValue = resjson[*iter];
                if(PathValue.isString())
                {
                    //only one filepath
                    string pathvalue=resjson[*iter].asString();
                    if(pathvalue.empty()) continue;
                    result->add_filelist(pathvalue);
                }
                else if(!PathValue.isNull() && PathValue.isArray())
                {//have mult filepath
                    for(int ind =0;ind<PathValue.size();ind++)
                    {
                        Json::Value PathValueEm = PathValue[ind];
                        if(PathValueEm.isNull() || !PathValueEm.isString())continue;

                        string pathvalue=PathValueEm.asString();
                        if(pathvalue.empty()) continue;
                        result->add_filelist(pathvalue);
                    }
                }
            }
        }
        catch(exception &e)
        {
            LOG(ERROR)<<e.what();
        }
}
bool Manager_Task::PUSHRemoteFiles(string info,string taskid,string indices,TaskResult& result)
{
    try
    {

//result file array
        if(result.filelist_size()>0)
        {
            for(int j=0;j<result.filelist_size();j++)
            {
                //readfile
                string filename = result.filelist(j);
                string fileext = strrchr(filename.c_str(), '.');
                if(fileext.empty()||fileext.rfind("json")==string::npos){continue;}

                string strread = ReadLocalFile(filename);
                if(strread.size()<1)    continue;

                Json::Value jsondata;
                jsondata["taskid"]=taskid;
                jsondata["task"]=info;
                jsondata["file"]=result.filelist(j);
                jsondata["json"]=strread.c_str();
                Json::FastWriter jfw;

                std::string strpostdata=jfw.write(jsondata);
                bool IsOK = Manager_ES::getInstance()->POSTTaskResult(indices,strpostdata);


            }



        }
    }
    catch(exception &e)
    {
        LOG(ERROR)<<"result file array"<<e.what();
    }
}
bool Manager_Task::PUSHRemoteDataCF( string info,TaskInfo* task,string strkey,string indices ,string resultjson)
{
    //-t fbmcontrol -k 133 -b eyJjb250cm9sRmlsZVBhdGgiOiIvaG9tZS90d2Qvc291cmNlL3NlcnZpY2V0YXNrL1BPQy9kb3dubG9hZC9mYWNlYm9va01lc3NhZ2UvY29udHJvbERhdGEvMzI3NDI0NDA1OEBxcS5jb20iLCJjb250YWN0UGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9jb250YWN0cy5qc29uIiwic3RhdHVzIjoib2siLCJtZXNzYWdlUGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9tZXNzYWdlLmpzb24ifQ==
    string body = b64_encode(resultjson);

    ostringstream sscmd;
    sscmd<< "./"+this->dcfexename+" ";
    sscmd<<" -t " << info << " -k "<< strkey << " -b " <<body;

    string DataClassifycmd;
    DataClassifycmd = sscmd.str();
    LOG(INFO)<<"runshell :"<<DataClassifycmd.c_str();
    string strret= RunShell(DataClassifycmd.c_str());
//    LOG(INFO)<< " => " << strret;
    if(strret.compare("-1")==0 || strret.size()<3) return true;

//            result.set_dataclassify(strret);

//result dataclassify
    if(strret.size()>10)
    {
        Json::Value jdataclassify;
        Json::Reader jread;
        jread.parse(strret,jdataclassify);
        if(!jdataclassify.isNull()&&jdataclassify.isArray())
        {
            for(int i=0;i<jdataclassify.size();i++)
            {
                Json::Value jelement = jdataclassify[i];
                Json::Value jsondata;
                jsondata["taskid"]=task->id();
                jsondata["task"]=info;
                string type =jelement["type"].isString()? jelement["type"].asString():"unknow";//message  or contacts
                jsondata[type]=jelement["body"];
                Json::FastWriter jfw;

                std::string strpostdata=jfw.write(jsondata);
                Manager_ES::getInstance()->POSTTaskResult(indices,strpostdata);

                task->set_datacount(task->datacount()+1);

            }

        }
    }
}
void Manager_Task::TaskLoops(TaskInfo* info)
{
    if(this->m_IsStop) return ;
    TaskInfo change_task;
    change_task.set_id(info->id());
//check task
    if(info->nodeid().compare(m_workID())!=0){info->set_nodeid(m_workID());}
/*    bool IsTimeOut = CheckTimeOut(info);//ptime and timenow
    if(IsTimeOut)
    {
//timeout
        LOG(INFO)<<"task timeout -> taskid:"<< info->id() << " taskclientid:" << info->nodeid();
        info->set_status(TaskInfo_TaskStatus_TimeOut);
        change_task->set_status(info->status());
    }*/

    bool IsComplete = info->progress()>=GetTaskTol(*info);
    if(IsComplete&&GetTaskTol(*info)>0)
    {
//complete
        LOG(INFO)<<"task completed -> taskid:"<< info->id() << " taskclientid:" << info->nodeid();
        time_t time_now; time(&time_now);
        info->set_etime(time_now);
        info->set_status(TaskInfo_TaskStatus_Complete);
        change_task.set_etime(info->etime());
        change_task.set_status(info->status());
    }

    bool IsRunned = info->progress()< GetTaskTol(*info);
    if(IsRunned)
    {
        LOG(INFO)<<"task IsRunned -> taskid:"<< info->id() << " taskprocess:" << info->progress();
// file is redy?
        if(!CheckSubsequent(*info))    TaskProcess(*info);
        if(CheckSubsequent(*info))
        {
            string strkey = GetKey(*info);
            string strtask = GetInfo(*info);
            string strApp = GetAppName(strtask);
            string restjson = ReadLocalFile(m_taskRstfile(strApp+strkey));

            TaskResult result;
            result.set_info(strApp);
            result.set_json(restjson);
            resultAddfiles(&result,restjson);

            string indices = "key" + strkey;
            bool bolputrst = PUSHRemoteResult(strApp,info->id(),indices,restjson);
            bool bolputfiles=PUSHRemoteFiles(strApp,info->id(),indices,result);
            bool bolputdatacf=PUSHRemoteDataCF(strApp,info,strkey,indices,restjson);

//            if(rst){
                if(remove(m_taskRstfile(strApp+strkey).c_str())==0)
                { LOG(INFO)<<"clean rstfile :" << m_taskRstfile(strApp+strkey); }
                else {   LOG(INFO)<<"clean rstfile error! :"<<m_taskRstfile(strApp+strkey); }
//            }




//process update
            info->set_progress(info->progress()+1);
            change_task.set_progress(info->progress());
            change_task.set_datacount(info->datacount());
        }

        this->WriteLocalTask(*info);
        Manager_ES::getInstance()->UpdateTaskInfo(info->id(),pb2json(change_task));
    }
}

bool Manager_Task::ReadLocalTask(TaskInfo* info)
{
    try
    {
    string strret = ReadLocalFile(m_taskInffile());

    if(strret.size()<=0) return false;
    json2pb(*info, strret);
    }
    catch(exception& e)
    {
        LOG(ERROR)<<"error json2pb :" <<m_taskInffile()<<e.what();
    }
    return true;
}

bool Manager_Task::WriteLocalTask(TaskInfo info)
{
    return WriteLocalFile(m_taskInffile().c_str(),pb2json(info));
}

bool Manager_Task::GetTaskInfo(TaskInfo* info)
{
    if(this->m_IsStop) return false;
//    sleep(10);
    int count_retry=3;
    bool bolret = false;
    for(int i=0;i<count_retry+1;i++) //for 4    retry 3
    {
//full local taskinfo
        ReadLocalTask(info);
//LOG(INFO)<<"local task is ready"<<endl;
        if(info->id().size()>1){bolret = true; break;}

//local is no task
        TaskInfo info;
//LOG(INFO)<<"local is no task"<<endl;
//remote task --> local
        string strtask = Manager_ES::getInstance()->GetTaskInfo();
        try
        {
            Json::Value jsonRoot; Json::Reader reader;
            if (!reader.parse(strtask, jsonRoot)) continue;
            Json::Value hitslist = jsonRoot["hits"]["hits"];
            Json::Value jsontask; Json::Value jsontaskid;
            if(!hitslist.isArray()) continue;
            for(int hiti=0;hiti<hitslist.size();hiti++)
            {
                Json::Value jsonite = hitslist[hiti];
                jsontaskid = jsonite["_id"];
                Json::Value source = jsonite["_source"];
                if(source.isObject()) { jsontask = source; break;  }
            }

            if(!jsontask.isObject()||!jsontaskid.isString()) continue;

            string sinfo;
            try
            {
                string taskid = jsontaskid.asString();
                info.set_id(taskid);
                sinfo = jsontask.toStyledString();
                json2pb(info,sinfo);
                info.set_id(taskid);
            }
            catch(exception &source)
            {
                 LOG(ERROR)<<source.what()<<sinfo;
            }
        }
        catch(exception& e)
        {
          LOG(ERROR)<<e.what()<<strtask;
        }

        if(info.id().size()<1) {  bolret = false;  /*LOG(INFO)<<"remote is no task"<<endl;*/}

    //update local taskinfo
        time_t time_now; time(&time_now);
        info.set_status(TaskInfo::Running);                                             //set task running
        info.set_nodeid(m_workID());                                                    //set clientid
        info.set_ptime(time_now);                                                       //set gettask time
        info.set_progress(0);

//save local taskinfo
        if(!WriteLocalTask(info)){            LOG(ERROR) << "workmanager write taskinfo error"<<endl;        }

//create update taskinfo
        TaskInfo change_task;
        change_task.set_id(info.id());
        change_task.set_status(info.status());
        change_task.set_ptime(info.ptime());
        change_task.set_nodeid(info.nodeid());
        info.set_progress(info.progress());

//update remote taskinfo
        string putjson = pb2json(change_task);
        Manager_ES::getInstance()->UpdateTaskInfo(info.id(),putjson);
        bolret =true;
    }
    return bolret;
}

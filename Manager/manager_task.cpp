#include <comm.h>
#include "manager_es.h"
#include "manager_task.h"
#include "manager_info.h"
#include <jsoncpp2pb.h>
#include "bin2ascii.h"
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include "taskunit.h"
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;
using google::protobuf::Message;
using namespace SCPROTO;
using namespace boost::algorithm;
Manager_Task* Manager_Task::m_taskMNG = new Manager_Task;
MyHealth* Manager_Task::b_info = new MyHealth;
bool Manager_Task::m_IsStop=true;
string Manager_Task::dcfexename ="dataclassify";


bool Manager_Task::CheckSubsequent(absTask* task)
{
    if(task->GetTaskTol()>0 && task->progress() < task->GetTaskTol())
    {
        string strtask = task->GetInfo();
        string strkey = task->GetKey();
        string strApp = task->GetAppName(strtask);
        string strret = ReadLocalFile(m_taskRstfile(strApp+strkey).c_str());
        if(strret.size()<=0) return false;
    }
    return true;
}
bool Manager_Task::CheckTask(absTask* task)
{
    bool bolret = false;
    if(task->t_task.info_size()<1)
    {
        task->t_task.set_status(TaskInfo_TaskStatus_Complete);
    }
    bolret=true;
    return bolret;
}

string Manager_Task::TaskBegin(string appname,string& args)
{
    string filepath;
    ostringstream ocmd;
    try
    {

        vector<string> SplitVec;
        split(SplitVec, args, is_any_of(" "), token_compress_on);
        for(vector<string>::iterator it =SplitVec.begin();it !=SplitVec.end();)
        {
            string str = *it;
            if(str.find("-base64=")!=string::npos)
            {
                replace_first(str, "-base64=", "");
                string b64str=str;

                srand((unsigned)time(NULL));
                string filedir = Manager_conf::getInstance()->netdiskpath() + appname+"/";
                if(access(filedir.c_str(),0)==-1)
                {
                    mkdir(filedir.c_str(),0755);

                }
                filepath = filedir + std::to_string(rand()%100000);

                stringstream otcmd;
                otcmd<< "echo '"<<b64str<<"' |tr '@' '\n' |base64 -d >"<<filepath;
                string strcmd = otcmd.str();
                RunShell(strcmd.c_str());
                //b64_decode2file(b64str,filepath);

                ++it;
                ocmd << "-f="<< filepath <<(it ==SplitVec.end()?"":" ");

            }
            else
            {
                ++it;
                ocmd << str<<(it == SplitVec.end()?"":" ");
            }
            args = ocmd.str();
        }
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what();

    }

    return filepath;
}
void Manager_Task::TaskProcess(absTask* task)
{
    string strcmd="";
    bool isnextkey=false;
    ostringstream ocmd;
    ocmd << "" +Manager_conf::getInstance()->pocpath();
    if(task->GetTaskTol()>0 && task->progress() <= task->GetTaskTol())
    {
        string strtask = task->GetInfo();
        vector<string> strsubarray =vector<string>();
        SplitString(strtask.c_str(),strsubarray,"::");
        string strApp = strtask;
        string strargs="";
        string stroarg="";
        bool isCrack=false;
        vector<string> vectorargs =vector<string>();
        if(strsubarray.size()>1)
        {
            strApp = strsubarray[0];

            for(int i=1;i<strsubarray.size();i++)
            {
                string strtmp=strsubarray[i];
                if(strtmp.empty()) continue;
                vector<string> strv =vector<string>();
                SplitString(strtmp.c_str(),strv," ");

                for(int it=0;it<strv.size();it++)
                {
                    vectorargs.push_back(strv[it]);
                }
            }
        }
        ostringstream args;
        for(int i=0;i<vectorargs.size();i++)
        {
            args<< (i==0?"":" ");
            string t = vectorargs[i];
            if(t.compare("-l")==0) isCrack =true;
            else{
                if(t.size()>3&&(t.substr(0,2).compare("-o")==0)) stroarg=t.substr(3);
            }
            args<<t;
        }
        strargs = args.str();
        string filename = TaskBegin(strApp,strargs);
        string strkey = task->GetKey();
        string strip = Manager_conf::getInstance()->eshost();
        ocmd << strApp <<" -ip="<< strip << " -t=" +strApp << " -k=" << strkey << " " <<strargs;
        strcmd = ocmd.str();
        string strret;
        bool Isretry=true;
        for(int in=0;in<MAXRETRY;in++)
        {

            LOG(INFO)<<"runshell :"<<strcmd.c_str();
            strret= RunShell(strcmd.c_str());
            if(strret.find_first_of("{")!=string::npos) strret = strret.substr(strret.find_first_of("{"));
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
                root["args"]=strargs;

//Crack
                if(isCrack){
                int key_size = task->t_task.key_size();
                int info_size = task->t_task.info_size();
                if(key_size>=1 && info_size>=1 && !root["LoginStatus"].isNull())
                {
                    root["user"]=strkey;
                    if(root["LoginStatus"].asBool())
                    {

                        root["pass"]=stroarg;
                        isnextkey=true;
                    }

                }
                }
                Json::FastWriter write;
                resultjson = write.write(root);

            }
            else
            {
                throw exception();
            }
        }
        catch(exception &e)
        {
            Json::Value tmp;
            tmp["status"]="error";
            tmp["message"]=strret;
            tmp["args"]=strargs;
            resultjson =tmp.toStyledString();
        }
        WriteLocalFile(m_taskRstfile(strApp+strkey).c_str(),resultjson);
        try{
        remove(filename.c_str());
        }
        catch(...){}
    }
    if(isnextkey){task->NextKey();}
    else{task->NextInfo();}
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
        time_t time_now; time(&time_now);
        jsondata["spidedate"] = std::to_string(time_now);
        jsondata["task"]=info;
        jsondata["taskid"]=taskid;
        jsondata["result"]=putjson.c_str();

        Json::FastWriter jfw;
        std::string strpostdata=jfw.write(jsondata);
        Manager_ES::getInstance()->POSTTaskResult(indices,info+"_r",strpostdata);
        ret = true;
    }
    catch(exception& e)
    {
        LOG(ERROR)<<"CURL ERROR :"<<indices<<e.what();
    }

    return ret;
}

bool Manager_Task::TaskAfter(absTask* task)
{
    if(task->t_task.status()!=TaskInfo_TaskStatus_Complete&&task->t_task.status()!=TaskInfo_TaskStatus_Stop)
    {
        return false;
    }
    if(remove(task->getfilename4task().c_str())==0) { LOG(INFO)<<"clean taskfile !"; }
    try
    {
        TaskInfo changetask;
        changetask.set_id(task->t_task.id());
        changetask.set_status(task->t_task.status());
        changetask.set_etime(task->t_task.etime());
        //task info set stat in ES
        Manager_ES::getInstance()->UpdateTaskInfo(task->t_task.id(),pb2json(changetask));
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
                std::size_t found = strkey.rfind("Path")!=string::npos?strkey.rfind("Path"):strkey.rfind("path");
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
                try
                {
                //readfile
                string filename = result.filelist(j);
                string fileext = strrchr(filename.c_str(), '.');
                if(fileext.empty()||fileext.rfind("json")==string::npos){continue;}

                string strread = ReadLocalFile(filename);
                if(strread.size()<1)    continue;

                Json::Value jsondata;
                time_t time_now; time(&time_now);
                jsondata["spidedate"] = std::to_string(time_now);
                jsondata["taskid"]=taskid;
                jsondata["task"]=info;
                jsondata["file"]=result.filelist(j);
                jsondata["json"]=strread.c_str();
                Json::FastWriter jfw;

                std::string strpostdata=jfw.write(jsondata);
                bool IsOK = Manager_ES::getInstance()->POSTTaskResult(indices,info,strpostdata);
                }
                catch(exception &e)
                {
                    LOG(ERROR)<<"result file array"<<e.what();
                }

            }



        }
    }
    catch(...)
    {

    }
    return true;

}
bool Manager_Task::PUSHRemoteDataCF( string info,TaskInfo* task,string strkey,string indices ,string resultjson)
{
    //-t fbmcontrol -k 133 -b eyJjb250cm9sRmlsZVBhdGgiOiIvaG9tZS90d2Qvc291cmNlL3NlcnZpY2V0YXNrL1BPQy9kb3dubG9hZC9mYWNlYm9va01lc3NhZ2UvY29udHJvbERhdGEvMzI3NDI0NDA1OEBxcS5jb20iLCJjb250YWN0UGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9jb250YWN0cy5qc29uIiwic3RhdHVzIjoib2siLCJtZXNzYWdlUGF0aCI6Ii9ob21lL3R3ZC9zb3VyY2Uvc2VydmljZXRhc2svUE9DL3Jlc3VsdC8zMjc0MjQ0MDU4QHFxLmNvbS9tZXNzYWdlLmpzb24ifQ==
    string body = b64_encode(resultjson);

    ostringstream sscmd;
    sscmd<< "./"+dcfexename+" ";
    sscmd<<" -t " << info << " -k "<< strkey << " -b " <<body;

    string DataClassifycmd;
    DataClassifycmd = sscmd.str();
    RunShell("echo \"\" >dataresult");//??initdataresult
    LOG(INFO)<<"runshell :"<<DataClassifycmd.c_str();
    RunShell(DataClassifycmd.c_str());

//    string put2dbcmd;
//    //sscmd.clear();
//    sscmd<<" -id "<<task->id();
//    put2dbcmd = sscmd.str();
//    LOG(INFO)<<"runshell :"<<put2dbcmd.c_str();
//    RunShell(put2dbcmd.c_str());


    string strret= ReadLocalFile("dataresult");//

    LOG(INFO)<< " => " << strret;
    if(strret.compare("-1")==0 || strret.size()<3) return true;

//            result.set_dataclassify(strret);

//result dataclassify
    if(strret.size()>10)
    {
//        task->set_datacount(task->datacount()+1);
        Json::Value jdataclassify;
        Json::Reader jread;
        jread.parse(strret,jdataclassify);
        if(!jdataclassify.isNull()&&jdataclassify.isArray())
        {
            for(int i=0;i<jdataclassify.size();i++)
            {
                Json::Value jelement = jdataclassify[i];
                Json::Value jsondata;
                time_t time_now; time(&time_now);
                jsondata["spidedate"] = std::to_string(time_now);
                jsondata["taskid"]=task->id();
                jsondata["task"]=info;
                string type =jelement["type"].isString()? jelement["type"].asString():"unknow";//message  or contacts
                jsondata[type]=jelement["body"];
                Json::FastWriter jfw;
                std::string strmessageid="";
                try{
                    //voxerid
                    string messageid = jelement["body"]["message_id"].asString();
                    if(messageid.empty()) messageid=jelement["body"]["docid"].asString();
                    strmessageid.append(messageid);
                }catch(...){}
                try{
                    //imo is no id
//                    string messageid = jelement["body"]["message_id"].asString();
//                    strmessageid.append(messageid);
                }catch(...){}
                std::string strpostdata=jfw.write(jsondata);
                bool puttrue = true;
		if(strmessageid.empty()||strmessageid.size()<2) puttrue = Manager_ES::getInstance()->POSTTaskResult(indices,info,strpostdata);
                else
                {
                    boost::to_lower(strmessageid);
                    boost::replace_all(strmessageid,"/","");
                    boost::replace_all(strmessageid,"\\","");
                    boost::replace_all(strmessageid,"$","");
                    puttrue = Manager_ES::getInstance()->POSTTaskResult(indices,info,strmessageid,strpostdata);
                }
                if(type.compare("message")==0 && puttrue ){  task->set_datacount(task->datacount()+1);}

            }

        }
    }
    return true;
}
bool Manager_Task::CheckTimeOut(absTask* task)
{
//    time_t time_now; time(&time_now);
//    long l = time_now - task->t_task.ptime();
//    if(l>TIME_OUT) return true;
    return false;
}

void Manager_Task::TaskLoops(absTask* task)
{
    if(m_IsStop) return ;





    TaskInfo change_task;
    change_task.set_id(task->t_task.id());
//check task
    if(task->t_task.nodeid().compare(m_workID())!=0){task->t_task.set_nodeid(m_workID());}

    bool IsTimeOut = CheckTimeOut(task);//ptime and timenow
    if(IsTimeOut)
    {
//timeout
        LOG(INFO)<<"task timeout -> taskid:"<< task->t_task.id() << " taskclientid:" << task->t_task.nodeid();
        task->t_task.set_status(TaskInfo_TaskStatus_TimeOut);
        change_task.set_status(task->t_task.status());
    }

    bool IsComplete = task->t_task.progress()>=task->GetTaskTol();
    if(IsComplete&&task->GetTaskTol()>0)
    {
//complete
        LOG(INFO)<<"task completed -> taskid:"<< task->t_task.id() << " taskclientid:" << task->t_task.nodeid()<<" p:t:"<<task->t_task.progress() << ":"<<task->GetTaskTol();
        time_t time_now; time(&time_now);
        task->t_task.set_etime(time_now);
        task->t_task.set_status(TaskInfo_TaskStatus_Complete);
        change_task.set_etime(task->t_task.etime());
        change_task.set_status(task->t_task.status());
    }

    bool IsRunned = task->progress()< task->GetTaskTol();
    if(IsRunned)
    {
        LOG(INFO)<<"task IsRunned -> taskid:"<< task->t_task.id() << " taskprocess:" << task->progress() <<" p:t:"<<task->t_task.progress() << ":"<<task->GetTaskTol();
// file is redy?
        if(!CheckSubsequent(task))    TaskProcess(task);
        if(CheckSubsequent(task))
        {
            string strkey = task->GetKey();
            string strtask = task->GetInfo();
            string strApp = task->GetAppName(strtask);
            string restjson = ReadLocalFile(m_taskRstfile(strApp+strkey));

            TaskResult result;

            result.set_info(strApp);
            result.set_json(restjson);
            resultAddfiles(&result,restjson);

            string indices = "key" + strkey;
            string taskid= task->t_task.id();
            bool bolputrst = PUSHRemoteResult(strApp,taskid,indices,restjson);
            bool bolputfiles=PUSHRemoteFiles(strApp,taskid,indices,result);
            bool bolputdatacf=PUSHRemoteDataCF(strApp,&task->t_task,strkey,indices,restjson);

//            if(rst){
                if(remove(m_taskRstfile(strApp+strkey).c_str())==0)
                { LOG(INFO)<<"clean rstfile :" << m_taskRstfile(strApp+strkey); }
                else {   LOG(INFO)<<"clean rstfile error! :"<<m_taskRstfile(strApp+strkey); }
//            }




//process update
            task->t_task.set_progress(task->t_task.progress()+task->getstep());
            change_task.set_progress(task->t_task.progress());
            change_task.set_datacount(task->t_task.datacount());
        }

        WriteLocalTask(task);
        Manager_ES::getInstance()->UpdateTaskInfo(task->t_task.id(),pb2json(change_task));
    }
}

bool Manager_Task::ReadLocalTask(absTask* task)
{
    string taskfile = task->getfilename4task();
    try
    {

    string strret = ReadLocalFile(taskfile);

    if(strret.size()<=0) return false;//no body
    task->t_task.Clear();
    json2pb(task->t_task, strret);
    }
    catch(exception& e)
    {
        LOG(ERROR)<<"error json2pb :" <<taskfile<<e.what();
    }
    return true;
}

bool Manager_Task::WriteLocalTask(absTask* task)
{
    string taskfile =task->getfilename4task();
    return WriteLocalFile(taskfile.c_str(),pb2json(task->t_task));
}
void Manager_Task::run()
{

}
/*
 *
 *
 *
 *
 */
void* RemotTaskAttribute(string taskid,string key,int& retcode)
{
    string strtask = Manager_ES::getInstance()->GetTaskInfo(taskid,retcode);
    try
    {
//        LOG(INFO)<<strtask;
        Json::Value jsonRoot; Json::Reader reader;
        if (!reader.parse(strtask, jsonRoot)) return NULL;
        Json::Value jsontask = jsonRoot["_source"];
        if(!jsontask.isObject()) return NULL;
        if(jsontask[key].isString()) return static_cast<void*>(new std::string(jsontask[key].asString()));
        if(!jsontask[key].isObject()) return NULL;
        return &jsontask[key];
    }
    catch(exception)
    {
      return NULL;
    }
}
bool Manager_Task::IsChangeRemotDelete(absTask* task)
{
    int retcode =0;
    void* obj = RemotTaskAttribute(task->t_task.id(),"status",retcode);
    if(retcode == 404)
    {
        task->t_task.set_status(TaskInfo::Complete);
        return true;
    }
    else
    {
        return false;
    }

}
bool Manager_Task::IsChangeRemotStop(absTask* task)
{
    int retcode =0;
    void* obj = RemotTaskAttribute(task->t_task.id(),"status",retcode);
    if(obj!=NULL)
    {
        TaskInfo_TaskStatus status = *(TaskInfo_TaskStatus*)obj;
//                cout<<status<<endl;
        if(status == TaskInfo::Stop)
        {
            task->t_task.set_status(TaskInfo::Stop);
            return true;
        }
    }
    return false;
}
bool Manager_Task::IsChangeTaskOwn(absTask* task)
{
    bool bolret = false;
    int retcode =0;
    void* obj = RemotTaskAttribute(task->t_task.id(),"nodeid",retcode);
    if(obj!=NULL)
    {
        std::string *sp = static_cast<std::string*>(obj);
        // You could use 'sp' directly, or this, which does a copy.

        if(sp!=NULL){
        std::string nodeid = *sp;
//                cout<<status<<endl;
        if(m_workID().compare(nodeid)==0)
        {
            bolret = false;
        }
        else
        {
            bolret=true;
            task->t_task.set_status(TaskInfo::Complete);
        }

        try{
        delete sp;
        }catch(...){}
        }
        return bolret;
    }
    return bolret;
}
void Manager_Task::TaskFilter(Json::Value & json)
{
    Json::Value newjson;

    Json::Value::Members mem = json.getMemberNames();
    Json::Value::Members::iterator iter;
    string id{"id"};
    string progress{"progress"};
    string status{"status"};
    string nodeid{"nodeid"};
    string info{"info"};
    string key={"key"};
//    string ctime{"ctime"};
//    string ptime{"ptime"};
//    string rtime{"rtime"};
//    string etime{"etime"};
//    string datacount{"datacount"};

    for (iter = mem.begin(); iter != mem.end(); iter++)
    {
        if(
            id.compare(*iter)==0
            ||progress.compare(*iter)==0
        //    ||ctime.compare(*iter)==0
        //    ||ptime.compare(*iter)==0
        //    ||rtime.compare(*iter)==0
        //    ||etime.compare(*iter)==0
            ||status.compare(*iter)==0
            ||nodeid.compare(*iter)==0
            ||info.compare(*iter)==0
            ||key.compare(*iter)==0
        //    ||datacount.compare(*iter)==0
            )
        {
            newjson[*iter]=json[*iter];
//            LOG(INFO)<<"forward "<<*iter<<"\t: "<<json[*iter];
        }
        else
        {
//            LOG(INFO)<<"drop "<<*iter<<"\t: "<<json[*iter];
        }

    }
    json.swap(newjson);


}
bool Manager_Task::GetTaskInfo(absTask* task)
{
    if(m_IsStop) return false;
//    sleep(10);
//    LOG(INFO)<<"GETTASKINFO";
    int count_retry=3;
    bool bolret = false;
    for(int i=0;i<count_retry+1;i++) //for 4    retry 3
    {
//full local taskinfo
        bool bolret = ReadLocalTask(task);
//LOG(INFO)<<"local task is ready"<<endl;
        if(task->t_task.id().size()>1)
        {
            if(IsChangeRemotDelete(task) || IsChangeRemotStop(task)){ bolret=false;break;}

            bolret = true;
            break;
        }


//local is no task
        TaskInfo* info=&task->t_task;
//LOG(INFO)<<"local is no task"<<endl;
//remote task --> local
        vector<string> vectask = Manager_ES::getInstance()->GetNewTaskId();

        if(vectask.empty()) break;
        LOG(INFO)<<"vectask size" << vectask.size();
        string strtaskid;
        bool havetask=false;
        for(vector<string>::iterator it =vectask.begin();it !=vectask.end();it++)
        {
            string str = *it;
            LOG(INFO)<<"TASKID "<<str;
            //es add doc lock
            bool islock = Manager_ES::getInstance()->createLock4taskid(str,m_workID());
            LOG(INFO)<<"LOCK TASKID "<<str<< " "<<m_workID();
            sleep(2);


            string lockinfo = Manager_ES::getInstance()->getLock4taskid(str);
            string strown="";
            Json::Value jsonlock; Json::Reader rd;
            try
            {
                rd.parse(lockinfo, jsonlock);
                strown = jsonlock["_source"]["process_id"].asString();
            }
            catch(...){ }
            if(islock||strown.compare(m_workID())==0)
            {
                strtaskid = str;
                havetask = true;
                break;
            }
        }

        if(!havetask) break;






        if(strtaskid.size()<1) {  bolret = false;  /*LOG(INFO)<<"remote is no task"<<endl;*/}






//create update taskinfo
        TaskInfo change_task;
        change_task.set_id(strtaskid);
        change_task.set_status(TaskInfo::Running);
        time_t time_now; time(&time_now);
        change_task.set_ptime(time_now);
        change_task.set_nodeid(m_workID());
//        info->set_progress(info->progress());

//update remote taskinfo
        string putjson = pb2json(change_task);
        int intret =Manager_ES::getInstance()->UpdateTaskInfo(strtaskid,putjson);
        if(intret==0)
        {
            int retcode =0;
            string strtask = Manager_ES::getInstance()->GetTaskInfo(strtaskid,retcode);
    //        LOG(INFO)<<"TASKINFO"<<strtask;
            try
            {
                Json::Value jsonRoot; Json::Reader reader;
                if (!reader.parse(strtask, jsonRoot)) break;
                task->version = jsonRoot["_version"].asInt();
                Json::Value jsontaskid=jsonRoot["_id"];
                Json::Value jsontask = jsonRoot["_source"];
                if(!jsontask.isObject()||!jsontaskid.isString()) break;



                string sinfo;
                try
                {
                    TaskFilter(jsontask);

                    sinfo = jsontask.toStyledString();
                    json2pb(*info,sinfo);


    ////update remote task status is run
    //                TaskInfo t_task;
    //                t_task.set_id(jsontaskid.asString());
    //                t_task.set_status(TaskInfo::Running);
    //                if(0!=Manager_ES::getInstance()->UpdateTaskInfo(t_task.id(),pb2json(t_task))) break;
    //                task->version+=1;
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
                    //es add doc lock
//save local taskinfo
        if(!WriteLocalTask(task)){            LOG(ERROR) << "workmanager write taskinfo error"<<endl;        }
        }
        else
        {
            bolret=false;
        }
        bolret =true;


        int tmpcount=0;
        while(!Manager_ES::getInstance()->deleteLock4taskid(strtaskid) && tmpcount<3)
        {
            LOG(ERROR)<<"delete lock "<<strtaskid<< " error ,sleep 2s";
            sleep(2);
            tmpcount++;
        }
        LOG(INFO)<<"UNLOCK TASKID "<<strtaskid<< " "<<task->t_task.nodeid();
    }
    return bolret;
}

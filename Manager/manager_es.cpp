#include "base.h"
#include "manager_es.h"
#include "cpr/cpr.h"
#include "urlcode.h"
#include <elasticlient/client.h>
#include<algorithm>
#include<jsoncpp/json/json.h>
using namespace std;
using namespace elasticlient;
Manager_ES* Manager_ES::esMNG = new Manager_ES;
std::vector<std::string> Manager_ES::m_hosts;
bool Manager_ES::IsAction()
{
    bool bolret =false;
    try
    {
        string strret = GetESInfo();
        if(strret.empty())return bolret;

        bolret = true;

    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what();
        bolret =false;
    }
    return bolret;
}
string Manager_ES::GetESInfo()
{
    string strret="";

    try
    {
        Client es(m_hosts);
        cpr::Response crsp = es.performRequest(elasticlient::Client::HTTPMethod::GET,"","");
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
//            LOG(ERROR)<<"error: curl: "<<crsp.url<<":"<<crsp.status_code<<":"<<crsp.error.message;
            return strret;
        }
        strret = crsp.text;
    }catch(exception &e)
    {
        LOG(ERROR)<<e.what();
    }

    return strret;
}
string Manager_ES::GetDevInfo(string nodeid)
{
    string strret="";
    Client es(m_hosts);
    cpr::Response crsp = es.get("devices","info",nodeid);
    if(crsp.status_code > 300 ||crsp.status_code <200)
    {
       LOG(ERROR)<<"error: curl: "<<crsp.url<<":"<<crsp.status_code<<":"<<crsp.error.message;
    }
    strret = crsp.text;
    return strret;
}
//push remote task to update
void Manager_ES::UpdateDevInfo(string nodeid,string putstrjson)
{

    Client es(m_hosts);
    try
    {


        string strpath ="devices/info/"+nodeid+"/_update";
        std::string strmsg="{ \"doc\" : " + putstrjson +" }";
        string strret ;
        cpr::Response crsp = es.index("devices","info",nodeid+"/_update",strmsg);
        if(crsp.status_code==404)
        {
            crsp = es.index("devices","info",nodeid,putstrjson);
        }
        else if(crsp.status_code > 300 ||crsp.status_code <200)
        {
            LOG(INFO)<<"error: curl: "<<"/"<<strpath<<strret;
            return ;
        }
        strret = crsp.text;
    }
    catch(exception& e)
    {
        LOG(ERROR)<<"CURL ERROR :"<<e.what();
//        cout<<"CURL ERROR :"<<e.what()<<endl;
    }
}
int Manager_ES::UpdateTaskInfo(string taskid,string putstrjson)
{
    return UpdateTaskInfo(taskid,putstrjson,-1);
}
int Manager_ES::UpdateTaskInfo(string taskid,string putstrjson,int version)
{

    if(taskid.empty()) return 1;

    try
    {
        Client es(m_hosts);
        std::string strmsg="{ \"doc\" : " + putstrjson +" }";
        std::string strver="?version="+to_string(version);
        cpr::Response crsp = es.index("task","taskinfo",taskid+"/_update"+(version==-1?"":strver),strmsg);
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
            LOG(INFO)<<"error: curl: "<<crsp.status_code<<"/"<<crsp.url<<crsp.text<<" --> "<<strmsg;
            return crsp.status_code;
        }
        return 0;
    }
    catch(exception& e)
    {
        LOG(ERROR)<<"CURL ERROR :"<<e.what();
    }
}
//get remote task
string Manager_ES::GetNewTaskId()
{
    std::string strret;
    cpr::Response crsp;
    try
    {
        Client es(m_hosts);
        crsp = es.get("task","taskinfo","_search?q=status:1&size=1");
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }
//        strret = crsp.text;
        string crsptext=crsp.text;
        try
        {
            Json::Value jsonRoot; Json::Reader reader;
            if (!reader.parse(crsptext.c_str(), jsonRoot)) return strret;
 //           task->version = jsonRoot
            Json::Value hitslist = jsonRoot["hits"]["hits"];
            Json::Value jsontask; Json::Value jsontaskid;
            if(!hitslist.isArray()) return strret;
            for(int hiti=0;hiti<hitslist.size();hiti++)
            {
                Json::Value jsonite = hitslist[hiti];
                jsontaskid = jsonite["_id"];
                if(jsontaskid.isString())
                {
                    return jsontaskid.asCString();
                }
            }
        }
        catch(...)
        {}
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what()<<crsp.url<<"--"<<strret;
    }
    return strret;
}
string Manager_ES::GetTaskInfo(string docid)
{
    std::string strret;
    cpr::Response crsp;
    try
    {
        Client es(m_hosts);
        crsp = es.get("task","taskinfo",docid);
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }
        strret = crsp.text;
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what()<<crsp.url<<"--"<<strret;
    }
    return strret;
}
bool Manager_ES::POSTTaskResult(string indices,string strpostdata)
{
    bool bolret=false;//lowercase
    transform(indices.begin(), indices.end(), indices.begin(), towlower);
    indices = UrlEncode(indices.c_str());
    cpr::Response crsp;
    try
    {
        Client es(m_hosts);
        crsp = es.index(indices,"data","?pretty=true",strpostdata);
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
            LOG(INFO)<<"error: curl: "<<"/"<<crsp.url<<crsp.text;
            return bolret;
        }
        bolret = true;
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what()<<crsp.status_code;
    }
    return bolret;
}

bool Manager_ES::createLock4taskid(string taskid,string ownid)
{
    LOG(INFO)<<"ES CREATELOCK,ID "<<taskid<<" own "<<ownid;
    bool bolret =false;
    if(taskid.empty()||ownid.empty()) return bolret;
    cpr::Response crsp;
    try
    {
        string url ="fs/lock/"+taskid+"/_create";
        string body="{ \"process_id\": \""+ownid+"\"    }";
        Client es(m_hosts);
        crsp = es.performRequest(elasticlient::Client::HTTPMethod::PUT,url,body);
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }
		else  bolret = true;
    }
    catch (const std::exception&)
    {
        
    }

    return bolret; 
}
bool Manager_ES::deleteLock4taskid(string taskid)
{
    bool bolret =false;
    if(taskid.empty()) return bolret;
    cpr::Response crsp;
    try
    {  

        Client es(m_hosts);
        crsp = es.remove("fs","lock",taskid);
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }else    bolret = true;
    }
    catch (const std::exception&)
    {
        
    }


    return bolret; 
}
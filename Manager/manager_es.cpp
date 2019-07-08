#include "base.h"
#include "manager_es.h"
#include "cpr/cpr.h"
#include "urlcode.h"
#include <elasticlient/client.h>
#include<algorithm>
#include<vector>
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
        if(m_hosts.size()<1) return "";
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
    string index = "dev"+nodeid;
    cpr::Response crsp = es.get(index,"data",nodeid);
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
        string index = "dev"+nodeid;


        string strpath =index+"/data/"+nodeid+"/_update";
        std::string strmsg="{ \"doc\" : " + putstrjson +" }";
        string strret ;
        cpr::Response crsp = es.index(index,"data",nodeid+"/_update",strmsg);
        if(crsp.status_code==404)
        {
            crsp = es.index(index,"data",nodeid,putstrjson);
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
        string hosts ="";

        for(vector<std::string>::iterator it = m_hosts.begin(); it != m_hosts.end();it++)
        {
            hosts +=*it;

        }
        LOG(ERROR)<<"CURL ERROR :"<<e.what() +hosts;
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

    }
    catch(exception& e)
    {
        LOG(ERROR)<<"CURL ERROR :"<<e.what();
    }
    return 0;
}
//get remote task
vector<string> Manager_ES::GetNewTaskId()
{
    std::string strret;
    std::vector<std::string> vecret =vector<string>();
    cpr::Response crsp;
    try
    {
        Client es(m_hosts);
        crsp = es.get("task","taskinfo","_search?q=status:1");
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }
//        strret = crsp.text;
        string crsptext=crsp.text;
        try
        {
            Json::Value jsonRoot; Json::Reader reader;
            if (!reader.parse(crsptext.c_str(), jsonRoot)) return vecret;
 //           task->version = jsonRoot
            Json::Value hitslist = jsonRoot["hits"]["hits"];
            Json::Value jsontask; Json::Value jsontaskid;
            if(!hitslist.isArray()) return vecret;
            for(int hiti=0;hiti<hitslist.size();hiti++)
            {
                Json::Value jsonite = hitslist[hiti];
                jsontaskid = jsonite["_id"];
                if(jsontaskid.isString())
                {
                    vecret.push_back(jsontaskid.asCString());
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
    return vecret;
}
string Manager_ES::GetTaskInfo(string docid,int& statuscode)
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
        Client es(m_hosts);

        crsp = es.performRequest(Client::HTTPMethod::POST,"fs/_refresh","");
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }

        string url ="fs/lock/"+taskid+"/_create";
        string body="{ \"process_id\": \""+ownid+"\"    }";

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

        crsp = es.performRequest(Client::HTTPMethod::POST,"fs/_refresh","");
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }


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

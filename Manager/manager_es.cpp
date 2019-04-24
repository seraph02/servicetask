#include "base.h"
#include "manager_es.h"
#include "cpr/cpr.h"
#include "urlcode.h"
#include <elasticlient/client.h>

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
        cpr::Response crsq = es.performRequest(elasticlient::Client::HTTPMethod::GET,"","");
        if(crsq.status_code > 300 ||crsq.status_code <200)
        {
    //                LOG(ERROR)<<"error: curl: "<<ESIP<<":"<<ESPort<<"/";
            return strret;
        }
        strret = crsq.text;
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
       LOG(ERROR)<<crsp.status_code<<"--"<<crsp.text;
    }
    strret = crsp.text;
    return strret;
}
//push remote task to update
void Manager_ES::UpdateDevInfo(string nodeid,string putstrjson)
{

    Client es(m_hosts);
//    //task info set stat in ES
//    string putstrjson;
//    try
//    {

//      putstrjson = pb2json(*info);


//    }
//    catch(exception &e)
//    {
//      LOG(ERROR)<<e.what()<<putstrjson;

//    }

    try
    {


        string strpath ="devices/info/"+nodeid+"/_update";
        std::string strmsg="{ \"doc\" : " + putstrjson +" }";
        string strret ;
        cpr::Response crsp = es.index("devices","info",nodeid+"/_update",strmsg);
//        cpr::Response crsq = es.performRequest(elasticlient::Client::HTTPMethod::POST,strpath,strmsg);

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
void Manager_ES::UpdateTaskInfo(string taskid,string putstrjson)
{

    if(taskid.empty()) return;

    try
    {
        Client es(m_hosts);

        string strpath ="task/taskinfo/"+taskid+"/_update";
        std::string strmsg="{ \"doc\" : " + putstrjson +" }";
        string strret ;
        cpr::Response crsp = es.index("task","taskinfo",taskid+"/_update",strmsg);
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
            LOG(INFO)<<"error: curl: "<<crsp.status_code<<"/"<<strpath<<crsp.text<<" --> "<<strmsg;
            return ;
        }
        strret = crsp.text;
    }
    catch(exception& e)
    {
        LOG(ERROR)<<"CURL ERROR :"<<e.what();
    }
}
//get remote task
string Manager_ES::GetTaskInfo()
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
    bool bolret=false;

    indices = UrlEncode(indices.c_str());
    cpr::Response crsq;
    try
    {
        Client es(m_hosts);
        crsq = es.index(indices,"data","?pretty=true",strpostdata);
        if(crsq.status_code > 300 ||crsq.status_code <200)
        {
            LOG(INFO)<<"error: curl: "<<"/"<<crsq.url<<crsq.text;
            return bolret;
        }
        bolret = true;
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what()<<crsq.status_code;
    }
    return bolret;
}

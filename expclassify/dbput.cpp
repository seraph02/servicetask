#include "dbput.h"
#include "cpr/cpr.h"
#include "urlcode.h"
#include <elasticlient/client.h>
#include<algorithm>
#include<vector>
#include<jsoncpp/json/json.h>
#include <glog/logging.h>
#include <jsoncpp2pb.h>
using namespace elasticlient;
using namespace std;
using namespace elasticlient;
dbput* dbput::esMNG = new dbput;
std::vector<std::string> dbput::m_hosts;
using namespace elasticlient;
string dbput::GetTaskInfo(string docid,int& statuscode)
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
int dbput::UpdateTaskInfo(string taskid,string putstrjson)
{
    return UpdateTaskInfo(taskid,putstrjson,-1);
}
int dbput::UpdateTaskInfo(string taskid,string putstrjson,int version)
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
        else
            sleep(2);

    }
    catch(exception& e)
    {
        LOG(ERROR)<<"CURL ERROR :"<<e.what();
    }
    return 0;
}
bool dbput::POSTTaskResult(string indices,string strpostdata)
{

    return POSTTaskResult(indices,"",strpostdata);
}
bool dbput::POSTTaskResult(string indices,string id,string strpostdata)
{
    bool bolret=false;//lowercase
    transform(indices.begin(), indices.end(), indices.begin(), towlower);
    indices = UrlEncode(indices.c_str());
    cpr::Response crsp;
    try
    {
        Client es(m_hosts);

        crsp = es.get(indices,"data","_search");
        if(crsp.status_code==404)
        {
            string newdevdate="{ \"settings\" : { \"index\" : { \"number_of_shards\" : 1}}}";
            try{
                es.performRequest(Client::HTTPMethod::PUT,indices,newdevdate);
            }
            catch(...){}
        }


        crsp = es.index(indices,"data",id+"?pretty=true",strpostdata);
        if(crsp.status_code==404)
        {

            crsp = es.index(indices,"data","?pretty=true",strpostdata);
        }else if(crsp.status_code > 300 ||crsp.status_code <200)
        {
            LOG(INFO)<<"error: curl: "<<"/"<<crsp.url<<"    /  "<<crsp.text;
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

#include "dbput.h"
#include "cpr/cpr.h"
#include "urlcode.h"
#include <elasticlient/client.h>
#include<algorithm>
#include<vector>
#include<jsoncpp/json/json.h>
#include <glog/logging.h>
#include <jsoncpp2pb.h>
#include "md5.h"
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
bool dbput::POSTTaskResult(string indices,string apptype,string strpostdata)
{

    return POSTTaskResult(indices,apptype,"",strpostdata);
}
string dbput::getJson2md5(string strpostdata,string apptype)
{
    string t_id;
    Json::Value resjson; Json::Reader jrd;
    //LOG(INFO)<<strpostdata;
    if(!jrd.parse(strpostdata,resjson)||resjson.isNull())
    {
        t_id = md5(strpostdata);
    }
    else if(apptype.find("fbmcontrol")!=string::npos && !resjson["message"].isNull())
    {
        Json::Value mesg = resjson["message"];
        if(!mesg["msgId"].isNull())
        {
            t_id = mesg["msgId"].asString();
        }
    }
    else{
        //resjson.removeMember("spidedate");
        resjson.removeMember("spidedate");
        if(apptype.find("control")!=string::npos && apptype.find("_r")==string::npos )
        {
            //LOG(INFO) << "remove_taskid:" << endl;
            resjson.removeMember("taskid");
        }
        Json::FastWriter jfw;
        std::string strtmpdata=jfw.write(resjson);
        LOG(INFO)<<strtmpdata;
        t_id = md5(strtmpdata);
        //LOG(INFO)<<"id: "<<id;
    }
    return t_id;
}
bool dbput::POSTTaskResult(string indices,string apptype,string id,string strpostdata)
{
    bool bolret=false;//lowercase
    string t_id ="";// id;
    transform(indices.begin(), indices.end(), indices.begin(), towlower);
    indices = UrlEncode(indices.c_str());

    t_id +=dbput::getJson2md5(strpostdata,apptype);

    string doc_id = apptype + t_id;

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
        if(!doc_id.empty())
        {
            cpr::Response crsp_getdoc = es.get(indices,"data",doc_id);
            if(crsp_getdoc.status_code==200)
            {
                bolret = false;
                return bolret;
            }
            crsp = es.index(indices,"data",doc_id+"?pretty=true",strpostdata);
            if(crsp.status_code==404)
            {


            }
        }

        bolret = true;
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what()<<crsp.status_code;
    }
    return bolret;
}

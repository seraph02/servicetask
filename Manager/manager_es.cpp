#include "base.h"
#include "md5.h"
#include "manager_es.h"
#include "cpr/cpr.h"
#include "urlcode.h"
#include <elasticlient/client.h>
#include<algorithm>
#include<vector>
#include<jsoncpp/json/json.h>
#include"myselfinfo.h"
#include<boost/algorithm/string.hpp>
using namespace std;
using namespace elasticlient;
Manager_ES* Manager_ES::esMNG = new Manager_ES;
std::vector<std::string> Manager_ES::m_hosts;
#include <arpa/inet.h>

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
        cpr::Response crsp = es.performRequest(elasticlient::Client::HTTPMethod::GET,"_cat/health","");
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
//            LOG(ERROR)<<"error: curl: "<<crsp.url<<":"<<crsp.status_code<<":"<<crsp.error.message;
            return strret;
        }
        strret = crsp.text;
    }catch(exception &e)
    {
        string hosts="";
        for(vector<std::string>::iterator it = m_hosts.begin(); it != m_hosts.end();it++)
        {
            hosts +=*it;

        }
        LOG(INFO)<<e.what()<<hosts;
    }

    return strret;
}
string Manager_ES::GetDevInfo(string nodeid)
{
    string strret="";
    Client es(m_hosts);
    string index = "dev"+nodeid;
    cpr::Response crsp = es.get(index,"data",nodeid);
    if(crsp.status_code ==404 )
    {
        string newdevdate="{ \"settings\" : { \"index\" : { \"number_of_shards\" : 1}}}";
        try{
            es.performRequest(Client::HTTPMethod::PUT,index,newdevdate);
        }
        catch(...){}
    }
    else if(crsp.status_code > 300 ||crsp.status_code <200)
    {
       LOG(ERROR)<<"error: curl: "<<crsp.url<<":"<<crsp.status_code<<":"<<crsp.error.message;
    }
    strret = crsp.text;
    return strret;
}
//push remote task to update
void Manager_ES::UpdateDevInfo(string nodeid,string putstrjson)
{
    if(nodeid.empty()) return;
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
//            LOG(INFO)<<putstrjson;
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
        else
            sleep(2);

    }
    catch(exception& e)
    {
        LOG(ERROR)<<"CURL ERROR :"<<e.what();
    }
    return 0;
}

string Manager_ES::IP2location(string ip)
{

    std::string strret;
    cpr::Response crsp;
    try
    {
        Client es(m_hosts);
        unsigned int ipnum=ntohl(inet_addr(ip.c_str()));

        stringstream bodystream;
        bodystream << "{   \"query\" : {     \"term\" : {       \"ipnum\": {         \"value\": "<<ipnum<<"      }    }  }}";
        string body = bodystream.str();
        crsp = es.search("ip2location","_doc",body);
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }
        strret = "";
        try
        {
            Json::Value jsonRoot; Json::Reader reader;
            if (!reader.parse(crsp.text, jsonRoot)) return "";
            Json::Value hitslist = jsonRoot["hits"]["hits"];

            if(hitslist.size()>0)
            {
                Json::Value jsonite = hitslist[0]["_source"];
                stringstream retstream;
                retstream<<jsonite["country_code"].asString()<<","<<jsonite["region_name"].asString();
                if(jsonite["country_code"].asString() == "-" || jsonite["region_name"].asString()=="-"){
                    string rep;
                    HttpGet("http://ip-api.com/json/" + ip,rep,30);
                    Json::Value jsonRep; Json::Reader reader;
                    if (!reader.parse(rep, jsonRep)) return retstream.str();
                    string status = jsonRep["status"].asString();
                    if(status == "success"){
                        string country_code = jsonRep["countryCode"].asString();
                        string city_name = jsonRep["city"].asString();
                        string region_name = jsonRep["regionName"].asString();
                        string country_name;
                        if(country_code=="HK"){
                            country_name = "China";
                        }else {
                            country_name = jsonRep["country"].asString();
                        }
                        Json::Value docUp;
                        docUp["country_code"] = Json::Value(country_code);
                        docUp["city_name"] = Json::Value(city_name);
                        docUp["region_name"] = Json::Value(region_name);
                        docUp["country_name"] = Json::Value(country_name);
                        Json::Value ipnum_jobj;
                        ipnum_jobj["gte"] = Json::Value(ipnum);
                        ipnum_jobj["lte"] = Json::Value(ipnum);
                        Json::Value location_jobj;
                        location_jobj["lat"] = Json::Value(jsonRep["lat"]);
                        location_jobj["lon"] = Json::Value(jsonRep["lon"]);
                        docUp["ipnum"] = ipnum_jobj;
                        docUp["location"] = location_jobj;
                        stringstream me_id_st;
                        me_id_st<<ipnum<<"_000_"<<ipnum;
                        string _doc_m_id = me_id_st.str();
                        crsp = es.index("ip2location","_doc",_doc_m_id,str1);
                        retstream.str("");
                        retstream<<country_code<<","<<city_name;
                        if(crsp.status_code > 300 ||crsp.status_code <200)
                        {
                           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
                        }
                    }
                }
                return retstream.str();
            }

        }
        catch(exception &cndoerr)
        {
            LOG(ERROR)<<cndoerr.what();
        }
    }
    catch(exception &e)
    {
        LOG(ERROR)<<e.what()<<crsp.url<<"--"<<strret;
    }
    return strret;
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
        crsp = es.get("task","taskinfo","_search?q=status:1&size=10");
        if(crsp.status_code ==404) {return vecret;}
        else if(crsp.status_code > 300 ||crsp.status_code <200)
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
            Json::Value jsontaskid;
            if(!hitslist.isArray()) return vecret;
            for(int hiti=0;hiti<hitslist.size();hiti++)
            {
                Json::Value jsonite = hitslist[hiti];

                //check task
                bool ishavenotwork = false;
                try
                {
                    Json::Value jsontaskinfo = jsonite["_source"]["info"];

                    //get info_list
//                    Json::FastWriter jfw;
//                    LOG(INFO)<<jfw.write(jsontaskinfo);
                    std::vector<string> info_list;
                    int sz = jsontaskinfo.size();
                    for (int i = 0; i < sz; ++i) {
                        string subtaskname = jsontaskinfo[i].asString();
                        vector<string> strsubarray =vector<string>();
                        SplitString(subtaskname,strsubarray,"::");
                        string strApp = "";
                        if(strsubarray.size()>0)
                        {
                            strApp = strsubarray.front();
                        }
                        subtaskname.c_str();
                        info_list.push_back(strApp);
                    }

                    //get do work list
                    std::vector<string> jobs_list;
                    MyHealth* healt = MyHealth::getInstance();
                    string jobs_str = healt->b_dev->jobs();
                    SplitString(jobs_str.c_str(),jobs_list,",");

                    //get cando_list
                    vector<string> cando_list;
//                    vector<string>::iterator retEndPos;
//                    cando_list.resize(info_list.size()+jobs_list.size());
//                    retEndPos =set_intersection(info_list.begin(),info_list.end(),jobs_list.begin(),jobs_list.end(),cando_list.begin());
//                    cando_list.resize(retEndPos-cando_list.begin());

                    for(int i = 0; i< info_list.size();i++)
                    {
                        string tt = info_list[i];
                        for(int j = 0; j< jobs_list.size();j++)
                        {
                            string ttj = jobs_list[j];
                            if(tt.compare(ttj) == 0)
                            {
                                cando_list.push_back(ttj);
                            }
                        }
                    }

                    LOG(INFO)<<"============";
                    LOG(INFO)<<"can do list:";
                    LOG(INFO)<<"------------";
                    for(int i = 0; i< cando_list.size();i++)
                    {
                        LOG(INFO)<<cando_list[i];
                    }
                    LOG(INFO)<<"============";
                    LOG(INFO)<<"jobs_list:";
                    LOG(INFO)<<"------------";
                    for(int i = 0; i< jobs_list.size();i++)
                    {
                        LOG(INFO)<<jobs_list[i];
                    }
                    LOG(INFO)<<"============";
                    LOG(INFO)<<"info_list:";
                    LOG(INFO)<<"------------";
                    for(int i = 0; i< info_list.size();i++)
                    {
                        LOG(INFO)<<info_list[i];
                    }
                    int cando_size = cando_list.size();
                    int info_size = info_list.size();
                    if( cando_size != info_size)
                    {
                        ishavenotwork = true;
                    }

                }
                catch(exception &cndoerr)
                {
                    LOG(ERROR)<<cndoerr.what();
                    ishavenotwork = true;
                }
                if(ishavenotwork)
                    continue;


                jsontaskid = jsonite["_id"];
                if(jsontaskid.isString())
                {
                    vecret.push_back(jsontaskid.asCString());
                }
            }
        }
        catch(exception &taske)
        {
            LOG(ERROR)<<"GET TASKLIST ERROR!:"<<taske.what();
        }
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
//bool Manager_ES::POSTTaskResult(string indices,string apptype,string strpostdata)
//{
//    return POSTTaskResult(indices,apptype,"",strpostdata);
//}

//bool Manager_ES::POSTTaskResult(string indices, string apptype, string id,string strpostdata)
//{
//    bool bolret=false;//lowercase
//    transform(indices.begin(), indices.end(), indices.begin(), towlower);
//    indices = UrlEncode(indices.c_str());
//    if(id.empty()||id.length()<2){
//        Json::Value resjson; Json::Reader jrd;
//        if(!jrd.parse(strpostdata,resjson)||resjson.isNull())
//        {
//            id = md5(strpostdata);
//        }
//        else{
//            resjson.removeMember("spidedate");
//	    if(apptype.find("control")!=string::npos && apptype.find("_r")==string::npos )
//            {
//                resjson.removeMember("taskid");
//            }
//            Json::FastWriter jfw;
//            std::string strtmpdata=jfw.write(resjson);
//            //LOG(INFO)<<strtmpdata;
//            id = md5(strtmpdata);
//            //LOG(INFO)<<"id: "<<id;
//        }

//    }
//    string doc_id = apptype + id;

//    cpr::Response crsp;
//    try
//    {
//        Client es(m_hosts);

//        crsp = es.get(indices,"data","_search");
//        if(crsp.status_code==404)
//        {
//            string newdevdate="{ \"settings\" : { \"index\" : { \"number_of_shards\" : 1}}}";
//            try{
//                es.performRequest(Client::HTTPMethod::PUT,indices,newdevdate);
//            }
//            catch(...){}
//        }
//        if(!id.empty())
//        {
//            cpr::Response crsp_getdoc = es.get(indices,"data",doc_id);
//            if(crsp_getdoc.status_code==200)
//            {
//                bolret = false;
//                return bolret;
//            }
//            crsp = es.index(indices,"data",doc_id+"?pretty=true",strpostdata);
//            if(crsp.status_code==404)
//            {


//            }
//        }

//        bolret = true;
//    }
//    catch(exception &e)
//    {
//        LOG(ERROR)<<e.what()<<crsp.status_code;
//    }
//    return bolret;
//}

bool Manager_ES::POSTBulkdata(string indices,string docid,string data)
{
    string type="data";
    bulk->indexDocument(type,docid, data);
    bulkcount++;
    if(bulkcount<10000) ;
    else
    {
        std::shared_ptr<elasticlient::Client> es = std::make_shared<elasticlient::Client>(
               std::vector<std::string>(m_hosts));
        elasticlient::Bulk bulkIndexer(es);
        size_t errors = bulkIndexer.perform(*bulk);
        bulk->clear();
        bulkcount=0;
    }

    return 0;
}


void Manager_ES::POSTBulkend(elasticlient::SameIndexBulkData& bulkdata)
{
    std::shared_ptr<elasticlient::Client> es = std::make_shared<elasticlient::Client>(
           std::vector<std::string>(m_hosts));
    elasticlient::Bulk bulkIndexer(es);
    size_t errors = bulkIndexer.perform(bulkdata);
    bulkdata.clear();
}

void Manager_ES::startbulk(string indices)
{

    Manager_ES::getInstance()->ChangeHosts(m_hosts);
    std::shared_ptr<elasticlient::Client> es = std::make_shared<elasticlient::Client>(
           std::vector<std::string>(m_hosts));
    std::make_shared<elasticlient::SameIndexBulkData>(indices,5000);

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
    catch (const std::exception& e)
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
string Manager_ES::getLock4taskid(string taskid)
{
    string strret="";
    if(taskid.empty()) return strret;

    cpr::Response crsp;
    try
    {

        Client es(m_hosts);

        crsp = es.get("fs","lock",taskid);
        if(crsp.status_code > 300 ||crsp.status_code <200)
        {
           LOG(ERROR)<<crsp.url<<"--"<<crsp.text;
        }
        strret = crsp.text;

    }
    catch (const std::exception&)
    {

    }



    return strret;
}

#include "dbput.h"
#include "cpr/cpr.h"
#include "urlcode.h"
#include <elasticlient/client.h>
#include<algorithm>
#include<vector>
#include<jsoncpp/json/json.h>
#include <glog/logging.h>
using namespace elasticlient;
using namespace std;
using namespace elasticlient;
dbput* dbput::esMNG = new dbput;
std::vector<std::string> dbput::m_hosts;
using namespace elasticlient;
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
